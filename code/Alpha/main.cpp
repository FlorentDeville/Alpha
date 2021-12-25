/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define NOMINMAX

#include <assert.h>
#include <cstdint>
#include <exception>
#include <chrono>
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// D3D12 extension library.
#include <d3dx12.h>

#include "CommandQueue.h"
#include "Helper.h"
#include "Mesh.h"
#include "RenderModule.h"
#include "Window.h"

bool g_VSync;

bool g_IsInitialized = false;

RenderModule* g_pRenderModule = nullptr;
Window* g_pWindow = nullptr;
Mesh* g_pMesh = nullptr;

//Shader signature
ID3D12RootSignature* g_pRootSignature;

//pipeline state
ID3D12PipelineState* g_pPipelineState;

D3D12_VIEWPORT g_viewport;
D3D12_RECT g_scissorRect;

float g_FoV;

DirectX::XMMATRIX g_model;
DirectX::XMMATRIX g_view;
DirectX::XMMATRIX g_projection;

bool g_contentLoaded;

struct PipelineStateStream
{
	CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
	CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
	CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
	CD3DX12_PIPELINE_STATE_STREAM_VS VS;
	CD3DX12_PIPELINE_STATE_STREAM_PS PS;
	CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
	CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
} pipelineStateStream;

static VertexPosColor g_Vertices[8] = {
	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
	{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f),	DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
	{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f),	DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),	DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
	{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
	{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f),	DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
	{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f),	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
	{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f),	DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
};

static uint16_t g_Indicies[36] =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};

void Update();
void Render();

template<typename T> constexpr const T& clamp(const T& val, const T& min, const T& max)
{
	return val < min ? min : val > max ? max : val;
}

void TransitionResource(ID3D12GraphicsCommandList2* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, beforeState, afterState);
	pCommandList->ResourceBarrier(1, &barrier);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!g_IsInitialized)
		return ::DefWindowProcW(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_PAINT:
		Update();
		Render();
		break;

	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;

			case VK_F11:
				g_pWindow->ToggleFullscreen();
				break;

			default:
				return ::DefWindowProcW(hWnd, message, wParam, lParam);
				break;
			}
		}
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_SIZE:
	{
		RECT clientRect = g_pWindow->GetWindowRectangle();
		int width = clientRect.right - clientRect.left;
		int height = clientRect.bottom - clientRect.top;

		// Don't allow 0 size swap chain back buffers.
		uint32_t uwidth = std::max(1, width);
		uint32_t uheight = std::max(1, height);

		if (g_pWindow->GetWidth() != uwidth || g_pWindow->GetHeight() != uheight)
		{
			g_pWindow->Resize(uwidth, uheight);
			g_pRenderModule->ResizeSwapChain(uwidth, uheight);
			g_pRenderModule->ResizeDepthBuffer(uwidth, uheight);
		}
	}
		break;

	default:
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void Update()
{
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();
	static auto start = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	if (elapsedSeconds > 1.0)
	{
		wchar_t buffer[500];
		auto fps = frameCounter / elapsedSeconds;
		swprintf_s(buffer, 500, L"FPS: %f\n", fps);
		OutputDebugString(buffer);

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}

	auto dt = clock.now() - start;
	double totalTimeElasped = dt.count() * 1e-9;
	// Update the model matrix.
	float angle = static_cast<float>(totalTimeElasped * 90.0);
	const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
	g_model = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));

	// Update the view matrix.
	const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
	const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
	g_view = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

	// Update the projection matrix.
	float aspectRatio = g_pWindow->GetWidth() / static_cast<float>(g_pWindow->GetHeight());
	g_projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(g_FoV), aspectRatio, 0.1f, 100.0f);
}

void Render()
{
	CommandQueue* pCommandQueue = g_pRenderModule->GetRenderCommandQueue();
	ID3D12GraphicsCommandList2* pCommandList = pCommandQueue->GetCommandList();

	ID3D12Resource* pBackBuffer = g_pRenderModule->m_pBackBuffers[g_pRenderModule->m_currentBackBufferIndex];

	//ID3D12GraphicsCommandList2* pCommandList = g_pCommandQueue->GetCommandList();

	// Clear the render target.
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		pCommandList->ResourceBarrier(1, &barrier);
	}

	FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), g_CurrentBackBufferIndex, g_RTVDescriptorSize);
	D3D12_CPU_DESCRIPTOR_HANDLE rtv = g_pRenderModule->GetRTV();
	pCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	

	// Clear the depth buffer
	//D3D12_CPU_DESCRIPTOR_HANDLE dsv = g_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = g_pRenderModule->GetDSV();
	float depthValue = 1.f;
	pCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depthValue, 0, 0, nullptr);
	//g_pRenderModule->PreRender();

	// Render the cube
	pCommandList->SetPipelineState(g_pPipelineState);
	pCommandList->SetGraphicsRootSignature(g_pRootSignature);

	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &g_pMesh->GetVertexBufferView());
	pCommandList->IASetIndexBuffer(&g_pMesh->GetIndexBufferView());

	pCommandList->RSSetViewports(1, &g_viewport);
	pCommandList->RSSetScissorRects(1, &g_scissorRect);

	//D3D12_CPU_DESCRIPTOR_HANDLE rtv = g_pRenderModule->GetRTV();
	//D3D12_CPU_DESCRIPTOR_HANDLE dsv = g_pRenderModule->GetDSV();
	pCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	// Update the MVP matrix
	DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
	mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, g_projection);
	pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

	pCommandList->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);

	//g_pRenderModule->Present();
	// Present
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		pCommandList->ResourceBarrier(1, &barrier);

		pCommandQueue->ExecuteCommandList(pCommandList);

		UINT syncInterval = g_VSync ? 1 : 0;
		UINT presentFlags = 0;//g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed(g_pRenderModule->m_pSwapChain->Present(syncInterval, presentFlags));

		uint64_t fenceValue = pCommandQueue->Signal();
		g_pRenderModule->m_currentBackBufferIndex = g_pRenderModule->m_pSwapChain->GetCurrentBackBufferIndex();

		pCommandQueue->WaitForFenceValue(fenceValue);
	}
}

bool LoadContent()
{
	g_pMesh = new Mesh();
	g_pMesh->LoadVertexAndIndexBuffer(g_Vertices, _countof(g_Vertices), g_Indicies, _countof(g_Indicies));

	// Load the vertex shader.
	ID3DBlob* pVertexShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(L"C:\\workspace\\Alpha\\code\\x64\\Debug\\VertexShader.cso", &pVertexShaderBlob));

	// Load the pixel shader.
	ID3DBlob* pPixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(L"C:\\workspace\\Alpha\\code\\x64\\Debug\\PixelShader.cso", &pPixelShaderBlob));

	// Create the vertex input layout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Create a root signature.
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(g_pRenderModule->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// Allow input layout and deny unnecessary access to certain pipeline stages.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	// A single 32-bit constant root parameter that is used by the vertex shader.
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	// Serialize the root signature.
	ID3DBlob* pRootSignatureBlob;
	ID3DBlob* pErrorBlob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, featureData.HighestVersion, &pRootSignatureBlob, &pErrorBlob));

	// Create the root signature.
	ThrowIfFailed(g_pRenderModule->GetDevice()->CreateRootSignature(0, pRootSignatureBlob->GetBufferPointer(), pRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&g_pRootSignature)));

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	pipelineStateStream.pRootSignature = g_pRootSignature;
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(pVertexShaderBlob);
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pPixelShaderBlob);
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};
	ThrowIfFailed(g_pRenderModule->GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&g_pPipelineState)));

	g_contentLoaded = true;

	return true;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	int width = 1080;
	int height = 789;

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	const wchar_t* pWindowClassName = L"DX12WindowClass";
	Window::RegisterWindowClass(hInstance, pWindowClassName, WndProc);
	g_pWindow = new Window();
	g_pWindow->Create(pWindowClassName, L"Alpha", width, height, hInstance);

	g_pRenderModule = new RenderModule();
	g_pRenderModule->Init(g_pWindow->GetWindowHandle(), width, height);

	g_IsInitialized = true;
	g_contentLoaded = false;
	LoadContent();

	g_pRenderModule->SetContentLoaded();

	g_pWindow->Show();

	{
		g_scissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
		g_viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));
		g_FoV = 45.f;
	}

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	g_pRenderModule->Shutdown();

	delete g_pMesh;
	delete g_pWindow;
	delete g_pRenderModule;

	return 0;
}
