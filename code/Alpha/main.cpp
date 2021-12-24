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
#include "Window.h"

const int g_NumFrames = 3;
ComPtr<ID3D12Device2> g_Device;

ComPtr<IDXGISwapChain4> g_SwapChain;
ComPtr<ID3D12Resource> g_BackBuffers[g_NumFrames];

ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
UINT g_RTVDescriptorSize;
UINT g_CurrentBackBufferIndex;
bool g_VSync;
bool g_TearingSupported;

bool g_IsInitialized = false;

CommandQueue* g_pCommandQueue = nullptr;
CommandQueue* g_pCopyCommandQueue = nullptr;
Window* g_pWindow = nullptr;


//Vertex buffer
ID3D12Resource* g_pVertexBuffer;
D3D12_VERTEX_BUFFER_VIEW g_vertexBufferView;

//Index buffer
ID3D12Resource* g_pIndexBuffer;
D3D12_INDEX_BUFFER_VIEW g_indexBufferView;

//Depth buffer
ID3D12Resource* g_pDepthBuffer;
ID3D12DescriptorHeap* g_pDSVHeap;

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

struct VertexPosColor
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;
};

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

static WORD g_Indicies[36] =
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
void ResizeSwapChain(uint32_t, uint32_t);
void ResizeDepthBuffer(uint32_t, uint32_t);

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
			ResizeSwapChain(uwidth, uheight);
			ResizeDepthBuffer(uwidth, uheight);
		}
	}
		break;

	default:
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void EnableDebugLayer()
{
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
#endif
}

ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp)
{
	ComPtr<IDXGIFactory4> dxgiFactory;
	UINT createFactoryFlag = 0;

#if defined(_DEBUG)
	createFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT res = CreateDXGIFactory2(createFactoryFlag, IID_PPV_ARGS(&dxgiFactory));
	ThrowIfFailed(res);

	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter4> dxgiAdapter4;

	if (useWarp)
	{
		res = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1));
		ThrowIfFailed(res);
		res = dxgiAdapter1.As(&dxgiAdapter4);
		ThrowIfFailed(res);
	}
	else
	{
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			//check if we can create a d3d12 device and pick the adapter with most memory
			
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) //no hardware
				continue;

			if (dxgiAdapterDesc1.DedicatedVideoMemory <= maxDedicatedVideoMemory) //less memory
				continue;

			res = D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(res))
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				res = dxgiAdapter1.As(&dxgiAdapter4);
				ThrowIfFailed(res);
			}
		}
	}

	return dxgiAdapter4;
}

ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter4> adapter)
{
	ComPtr<ID3D12Device2> d3d12Device2;
	HRESULT res = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2));
	ThrowIfFailed(res);

#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	res = d3d12Device2.As(&pInfoQueue);
	ThrowIfFailed(res);

	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
#endif

	return d3d12Device2;
}

bool CheckTearingSupport()
{
	BOOL allowTearing = true;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
	// graphics debugging tools which will not support the 1.5 factory interface 
	// until a future update.
	ComPtr<IDXGIFactory4> factory4;
	HRESULT res = CreateDXGIFactory1(IID_PPV_ARGS(&factory4));
	if (SUCCEEDED(res))
	{
		ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory4.As(&factory5)))
		{
			res = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
			if (FAILED(res))
			{
				allowTearing = false;
			}
		}
	}

	return allowTearing;
}

ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd, ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
{
	ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT res = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4));
	ThrowIfFailed(res);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	ComPtr<IDXGISwapChain1> swapChain1;
	res = dxgiFactory4->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
	ThrowIfFailed(res);

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

	return dxgiSwapChain4;
}

ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
{
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

	return descriptorHeap;
}

void UpdateRenderTargetViews(ComPtr<ID3D12Device2> device, ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap)
{
	UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < g_NumFrames; ++i)
	{
		ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

		g_BackBuffers[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
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
	auto backBuffer = g_BackBuffers[g_CurrentBackBufferIndex];

	ComPtr<ID3D12GraphicsCommandList2> pCommandList = g_pCommandQueue->GetCommandList();

	// Clear the render target.
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		pCommandList->ResourceBarrier(1, &barrier);
	}

	FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), g_CurrentBackBufferIndex, g_RTVDescriptorSize);

	pCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	

	// Clear the depth buffer
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = g_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
	float depthValue = 1.f;
	pCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depthValue, 0, 0, nullptr);
	

	// Render the cube
	pCommandList->SetPipelineState(g_pPipelineState);
	pCommandList->SetGraphicsRootSignature(g_pRootSignature);

	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &g_vertexBufferView);
	pCommandList->IASetIndexBuffer(&g_indexBufferView);

	pCommandList->RSSetViewports(1, &g_viewport);
	pCommandList->RSSetScissorRects(1, &g_scissorRect);

	pCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	// Update the MVP matrix
	DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
	mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, g_projection);
	pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

	pCommandList->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);

	// Present
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		pCommandList->ResourceBarrier(1, &barrier);

		g_pCommandQueue->ExecuteCommandList(pCommandList);

		UINT syncInterval = g_VSync ? 1 : 0;
		UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed(g_SwapChain->Present(syncInterval, presentFlags));

		uint64_t fenceValue = g_pCommandQueue->Signal();
		g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

		g_pCommandQueue->WaitForFenceValue(fenceValue);
	}
}

void ResizeSwapChain(uint32_t width, uint32_t height)
{
	// Flush the GPU queue to make sure the swap chain's back buffers
	// are not being referenced by an in-flight command list.
	g_pCopyCommandQueue->Flush();
	g_pCommandQueue->Flush();

	for (int i = 0; i < g_NumFrames; ++i)
	{
		// Any references to the back buffers must be released
		// before the swap chain can be resized.
		g_BackBuffers[i].Reset();
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	ThrowIfFailed(g_SwapChain->GetDesc(&swapChainDesc));
	ThrowIfFailed(g_SwapChain->ResizeBuffers(g_NumFrames, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

	g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

	UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap);

	wchar_t buffer[500];
	swprintf_s(buffer, 500, L"Resized swap chain %d %d\n", width, height);
	OutputDebugString(buffer);
}

void UpdateBufferResource(ID3D12GraphicsCommandList2* pCommandList, ID3D12Resource** pDestinationResource,
	ID3D12Resource** pIntermediateResource, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags)
{
	size_t bufferSize = numElements * elementSize;

	// Create a committed resource for the GPU resource in a default heap.
	D3D12_HEAP_PROPERTIES heapProrperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
	HRESULT res = g_Device->CreateCommittedResource(&heapProrperty, D3D12_HEAP_FLAG_NONE, &resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pDestinationResource));

	ThrowIfFailed(res);

	// Create an committed resource for the upload.
	if (bufferData)
	{
		D3D12_HEAP_PROPERTIES heapPropertyInt = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDescInt = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		res = g_Device->CreateCommittedResource(
			&heapPropertyInt,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescInt,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pIntermediateResource));

		ThrowIfFailed(res);

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = bufferData;
		subresourceData.RowPitch = bufferSize;
		subresourceData.SlicePitch = subresourceData.RowPitch;

		UpdateSubresources(pCommandList, *pDestinationResource, *pIntermediateResource, 0, 0, 1, &subresourceData);
	}
}

void ResizeDepthBuffer(uint32_t width, uint32_t height)
{
	if (g_contentLoaded)
	{
		// Flush any GPU commands that might be referencing the depth buffer.
		g_pCopyCommandQueue->Flush();
		g_pCommandQueue->Flush();

		// Resize screen dependent resources.
		// Create a depth buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		ThrowIfFailed(g_Device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&g_pDepthBuffer)
		));

		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		g_Device->CreateDepthStencilView(g_pDepthBuffer, &dsv, g_pDSVHeap->GetCPUDescriptorHandleForHeapStart());

		wchar_t buffer[500];
		swprintf_s(buffer, 500, L"Resized depth buffer %d %d\n", width, height);
		OutputDebugString(buffer);
	}
}

bool LoadContent()
{
	ComPtr<ID3D12GraphicsCommandList2> pCommandList = g_pCopyCommandQueue->GetCommandList();

	ID3D12Resource* pIntermediateVertexBuffer;
	UpdateBufferResource(pCommandList.Get(), &g_pVertexBuffer, &pIntermediateVertexBuffer, _countof(g_Vertices), sizeof(VertexPosColor), g_Vertices, D3D12_RESOURCE_FLAG_NONE);

	// Create the vertex buffer view.
	g_vertexBufferView.BufferLocation = g_pVertexBuffer->GetGPUVirtualAddress();
	g_vertexBufferView.SizeInBytes = sizeof(g_Vertices);
	g_vertexBufferView.StrideInBytes = sizeof(VertexPosColor);

	ID3D12Resource* pIntermediateIndexBuffer;
	UpdateBufferResource(pCommandList.Get(), &g_pIndexBuffer, &pIntermediateIndexBuffer, _countof(g_Indicies), sizeof(WORD), g_Indicies, D3D12_RESOURCE_FLAG_NONE);

	// Create index buffer view.
	g_indexBufferView.BufferLocation = g_pIndexBuffer->GetGPUVirtualAddress();
	g_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	g_indexBufferView.SizeInBytes = sizeof(g_Indicies);

	// Create the descriptor heap for the depth-stencil view.
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(g_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&g_pDSVHeap)));

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
	if (FAILED(g_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
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
	ThrowIfFailed(g_Device->CreateRootSignature(0, pRootSignatureBlob->GetBufferPointer(), pRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&g_pRootSignature)));

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
	ThrowIfFailed(g_Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&g_pPipelineState)));

	uint64_t fenceValue = g_pCopyCommandQueue->ExecuteCommandList(pCommandList);
	g_pCopyCommandQueue->WaitForFenceValue(fenceValue);

	g_contentLoaded = true;

	// Resize/Create the depth buffer.
	ResizeDepthBuffer(g_pWindow->GetWidth(), g_pWindow->GetHeight());

	return true;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	int width = 1080;
	int height = 789;

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	EnableDebugLayer();

	g_TearingSupported = CheckTearingSupport();

	const wchar_t* pWindowClassName = L"DX12WindowClass";
	Window::RegisterWindowClass(hInstance, pWindowClassName, WndProc);
	g_pWindow = new Window();
	g_pWindow->Create(pWindowClassName, L"Alpha", width, height, hInstance);

	ComPtr<IDXGIAdapter4> dxgiAdapter4 = GetAdapter(false);
	g_Device = CreateDevice(dxgiAdapter4);
	g_pCommandQueue = new CommandQueue(g_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	g_pCopyCommandQueue = new CommandQueue(g_Device, D3D12_COMMAND_LIST_TYPE_COPY);

	g_SwapChain = CreateSwapChain(g_pWindow->GetWindowHandle(), g_pCommandQueue->GetD3D12CommandQueue(), width, height, g_NumFrames);
	g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();
	g_RTVDescriptorHeap = CreateDescriptorHeap(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_NumFrames);
	g_RTVDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap);

	g_IsInitialized = true;
	g_contentLoaded = false;
	LoadContent();

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

	// Make sure the command queue has finished all commands before closing.
	g_pCommandQueue->Flush();

	delete g_pCommandQueue;
	delete g_pWindow;

	return 0;
}
