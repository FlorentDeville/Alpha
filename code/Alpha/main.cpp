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
#include "MeshMgr.h"
#include "RenderModule.h"
#include "Window.h"

bool g_VSync;

bool g_IsInitialized = false;

RenderModule* g_pRenderModule = nullptr;
Window* g_pWindow = nullptr;
MeshMgr* g_pMeshMgr = nullptr;

MeshId g_CubeMeshId;
MeshId g_QuadMeshId;

float g_FoV;

DirectX::XMMATRIX g_model;
DirectX::XMMATRIX g_view;
DirectX::XMMATRIX g_projection;

bool g_contentLoaded;

bool g_perspectiveRendering = true;

float g_posX = 0;
float g_posY = 0;

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

static float mul = 1;
static VertexPosColor g_Quad[4] =
{
	{ DirectX::XMFLOAT3(-0.5f * mul, 0.5f * mul, 0.f),	DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // top left
	{ DirectX::XMFLOAT3(0.5f * mul, 0.5f * mul, 0.f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // top right
	{ DirectX::XMFLOAT3(0.5f * mul, -0.5f * mul, 0.f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // bottom right
	{ DirectX::XMFLOAT3(-0.5f * mul, -0.5f * mul, 0.f),	DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // bottom left
};

static uint16_t g_QuadIndices[6]
{
	0, 1, 2,
	2, 3, 0
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

			case VK_NUMPAD0:
				g_perspectiveRendering = !g_perspectiveRendering;
				break;

			case VK_UP:
				++g_posY;
				break;

			case VK_DOWN:
				--g_posY;
				break;

			case VK_LEFT:
				--g_posX;
				break;

			case VK_RIGHT:
				++g_posX;
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

			g_pRenderModule->m_viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(uwidth), static_cast<float>(uheight));
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
	//float angle = 0;
	const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
	g_model = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));

	// Update the view matrix.
	const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
	const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
	g_view = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

	// Update the projection matrix.
	float aspectRatio = g_pWindow->GetWidth() / static_cast<float>(g_pWindow->GetHeight());
	if (g_perspectiveRendering)
	{				
		float nearDistance = 0.1f;
		float fovRad = DirectX::XMConvertToRadians(g_FoV);
		g_projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearDistance, 100.0f);
	}
	else
	{
		float w = 20.f;
		float h = w / aspectRatio;
		g_projection = DirectX::XMMatrixOrthographicLH(w, h, 0.1f, 100.f);
	}
}

void Render()
{
	CommandQueue* pCommandQueue = g_pRenderModule->GetRenderCommandQueue();
	ID3D12GraphicsCommandList2* pCommandList = pCommandQueue->GetCommandList();

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = g_pRenderModule->GetRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = g_pRenderModule->GetDSV();

	g_pRenderModule->PreRender(pCommandList);

	// Render the cube
	//if(false)
	{
		//// Update the MVP matrix
		DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
		mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, g_projection);
		g_pRenderModule->Render(pCommandList, g_CubeMeshId, mvpMatrix);
	}
	

	// Render the quad
	//if(false)
	{
		//dimension : 100 * 20 pixels
		float width = 100;
		float height = 20;
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, height, 0);

		//position : top left corner
		float windowWidth = static_cast<float>(g_pWindow->GetWidth());
		float windowHeight = static_cast<float>(g_pWindow->GetHeight());

		float x = width * 0.5f - windowWidth * 0.5f + g_posX;
		float y = windowHeight * 0.5f - height * 0.5f + g_posY;

		DirectX::XMMATRIX position = DirectX::XMMatrixTranslation(x, y, 1);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 0, 10, 1), DirectX::XMVectorSet(0, 1, 0, 1));

		float projWidth = static_cast<float>(windowWidth);
		float projHeight = static_cast<float>(windowHeight);
		DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(projWidth, projHeight , 0.1f, 100.f);

		DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(scale, position);
		mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, view);
		mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);

		g_pRenderModule->Render(pCommandList, g_QuadMeshId, mvpMatrix);
	}

	g_pRenderModule->PostRender(pCommandList);
}

bool LoadContent()
{
	Mesh* pCubeMesh = nullptr;
	g_pMeshMgr->CreateMesh(&pCubeMesh, g_CubeMeshId);
	pCubeMesh->LoadVertexAndIndexBuffer(g_Vertices, _countof(g_Vertices), g_Indicies, _countof(g_Indicies));

	Mesh* pQuadMesh = nullptr;
	g_pMeshMgr->CreateMesh(&pQuadMesh, g_QuadMeshId);
	pQuadMesh->LoadVertexAndIndexBuffer(g_Quad, _countof(g_Quad), g_QuadIndices, _countof(g_QuadIndices));

	g_pRenderModule->InitRootSignature();
	g_pRenderModule->InitPipelineState();
	
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

	g_pMeshMgr = new MeshMgr();

	g_IsInitialized = true;
	g_contentLoaded = false;
	LoadContent();

	g_pWindow->Show();

	{
		g_pRenderModule->m_scissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
		g_pRenderModule->m_viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));
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

	delete g_pMeshMgr;
	delete g_pWindow;
	delete g_pRenderModule;

	return 0;
}
