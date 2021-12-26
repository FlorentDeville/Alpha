/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "RenderModule.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>

#include "CommandQueue.h"
#include "Helper.h"

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

RenderModule::RenderModule()
	: m_pDevice(nullptr)
	, m_pSwapChain(nullptr)
	, m_pRTVDescriptorHeap(nullptr)
	, m_pDSVDescriptorHeap(nullptr)
	, m_RTVDescriptorSize(0)
	, m_allowTearing(false)
	, m_pRenderCommandQueue(nullptr)
	, m_pCopyCommandQueue(nullptr)
	, m_currentBackBufferIndex(0)
	, m_vSync(true)
	, m_pDepthBuffer(nullptr)
#if defined(_DEBUG)
	, m_pDebugInterface(nullptr)
#endif
{}

RenderModule::~RenderModule()
{}

void RenderModule::Init(HWND hWindow, uint32_t width, uint32_t height)
{
	EnableDebugLayer();

	CheckTearingSupport();
	
	IDXGIAdapter4* pAdapter = GetAdapter(false);
	CreateDevice(pAdapter);
	pAdapter->Release();

	

	m_pRenderCommandQueue = new CommandQueue(m_pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_pCopyCommandQueue = new CommandQueue(m_pDevice, D3D12_COMMAND_LIST_TYPE_COPY);

	CreateSwapChain(hWindow, m_pRenderCommandQueue->GetD3D12CommandQueue(), width, height, m_numFrames);
	m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	CreateRTVDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_numFrames);
	CreateDSVDescriptorHeap();

	m_RTVDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	UpdateRenderTargetViews();

	//Resize also create the depth buffer
	ResizeDepthBuffer(width, height);
}

void RenderModule::Shutdown()
{
	m_pRenderCommandQueue->Flush();
	m_pCopyCommandQueue->Flush();

	delete m_pRenderCommandQueue;
	delete m_pCopyCommandQueue;

#if defined(_DEBUG)
	m_pDebugInterface->Release();
#endif

	for (ID3D12Resource* pBackBuffer : m_pBackBuffers)
		pBackBuffer->Release();

	m_pDepthBuffer->Release();
	m_pDSVDescriptorHeap->Release();
	m_pRTVDescriptorHeap->Release();
	m_pSwapChain->Release();
	m_pDevice->Release();
	ReportLiveObject();
}

void RenderModule::PreRender(ID3D12GraphicsCommandList2* pCommandList)
{
	ID3D12Resource* pBackBuffer = m_pBackBuffers[m_currentBackBufferIndex];

	// Clear the render target.
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		pCommandList->ResourceBarrier(1, &barrier);
	}

	FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_RTVDescriptorSize);

	pCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);


	// Clear the depth buffer
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	float depthValue = 1.f;
	pCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depthValue, 0, 0, nullptr);
}

void RenderModule::PostRender(ID3D12GraphicsCommandList2* pCommandList)
{
	//final command : switch the back buffer to present state
	ID3D12Resource* pBackBuffer = m_pBackBuffers[m_currentBackBufferIndex];
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	pCommandList->ResourceBarrier(1, &barrier);

	//run the command list
	m_pRenderCommandQueue->ExecuteCommandList(pCommandList);

	//present
	UINT syncInterval = m_vSync ? 1 : 0;
	UINT presentFlags = m_allowTearing && !m_vSync? DXGI_PRESENT_ALLOW_TEARING : 0;
	ThrowIfFailed(m_pSwapChain->Present(syncInterval, presentFlags));

	//wait for the commands to be run
	uint64_t fenceValue = m_pRenderCommandQueue->Signal();
	m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	m_pRenderCommandQueue->WaitForFenceValue(fenceValue);
}

void RenderModule::ResizeSwapChain(uint32_t width, uint32_t height)
{
	// Flush the GPU queue to make sure the swap chain's back buffers
	// are not being referenced by an in-flight command list.
	m_pCopyCommandQueue->Flush();
	m_pRenderCommandQueue->Flush();

	for (int ii = 0; ii < m_numFrames; ++ii)
	{
		// Any references to the back buffers must be released
		// before the swap chain can be resized.
		m_pBackBuffers[ii]->Release();
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	ThrowIfFailed(m_pSwapChain->GetDesc(&swapChainDesc));
	ThrowIfFailed(m_pSwapChain->ResizeBuffers(m_numFrames, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

	m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	UpdateRenderTargetViews();

	wchar_t buffer[500];
	swprintf_s(buffer, 500, L"Resized swap chain %d %d\n", width, height);
	OutputDebugString(buffer);
}

void RenderModule::ResizeDepthBuffer(uint32_t width, uint32_t height)
{
	//if (!m_contentLoaded)
	//	return;

	// Flush any GPU commands that might be referencing the depth buffer.
	m_pCopyCommandQueue->Flush();
	m_pRenderCommandQueue->Flush();

	// Resize screen dependent resources.
	// Create a depth buffer.
	D3D12_CLEAR_VALUE optimizedClearValue = {};
	optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	optimizedClearValue.DepthStencil = { 1.0f, 0 };

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	
	if (m_pDepthBuffer)
		m_pDepthBuffer->Release();

	ThrowIfFailed(m_pDevice->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optimizedClearValue,
		IID_PPV_ARGS(&m_pDepthBuffer)
	));

	// Update the depth-stencil view.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
	dsv.Format = DXGI_FORMAT_D32_FLOAT;
	dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv.Texture2D.MipSlice = 0;
	dsv.Flags = D3D12_DSV_FLAG_NONE;

	m_pDevice->CreateDepthStencilView(m_pDepthBuffer, &dsv, m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	wchar_t buffer[500];
	swprintf_s(buffer, 500, L"Resized depth buffer %d %d\n", width, height);
	OutputDebugString(buffer);

}

CommandQueue* RenderModule::GetRenderCommandQueue()
{
	return m_pRenderCommandQueue;
}

CommandQueue* RenderModule::GetCopyCommandQueue()
{
	return m_pCopyCommandQueue;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderModule::GetRTV()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_RTVDescriptorSize);
	return rtv;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderModule::GetDSV()
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	return dsv;
}

ID3D12Device2* RenderModule::GetDevice()
{
	return m_pDevice;
}

void RenderModule::ReportLiveObject()
{
#if defined(_DEBUG)
	IDXGIDebug1* pDxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDxgiDebug))))
	{
		pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));

		pDxgiDebug->Release();
	}
#endif
}

void RenderModule::CreateDevice(IDXGIAdapter4* pAdapter)
{
	ID3D12Device2* pD3d12Device2;
	HRESULT res = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pD3d12Device2));
	ThrowIfFailed(res);

#if defined(_DEBUG)
	ID3D12InfoQueue* pInfoQueue;
	res = pD3d12Device2->QueryInterface(&pInfoQueue);
	ThrowIfFailed(res);

	//pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	//pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	//pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

	pInfoQueue->Release();
#endif

	m_pDevice = pD3d12Device2;
}

IDXGIAdapter4* RenderModule::GetAdapter(bool useWarp)
{
	IDXGIFactory4* pDxgiFactory;
	UINT createFactoryFlag = 0;

#if defined(_DEBUG)
	createFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT res = CreateDXGIFactory2(createFactoryFlag, IID_PPV_ARGS(&pDxgiFactory));
	ThrowIfFailed(res);

	IDXGIAdapter1* pDxgiAdapter1;
	IDXGIAdapter4* pDxgiAdapter4 = nullptr;

	if (useWarp)
	{
		res = pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pDxgiAdapter1));
		ThrowIfFailed(res);
		res = pDxgiAdapter1->QueryInterface(&pDxgiAdapter4);
		ThrowIfFailed(res);

		pDxgiAdapter1->Release();
	}
	else
	{
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; pDxgiFactory->EnumAdapters1(i, &pDxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			pDxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			//check if we can create a d3d12 device and pick the adapter with most memory

			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) //no hardware
			{
				pDxgiAdapter1->Release();
				continue;
			}
				
			if (dxgiAdapterDesc1.DedicatedVideoMemory <= maxDedicatedVideoMemory) //less memory
			{
				pDxgiAdapter1->Release();
				continue;
			}

			res = D3D12CreateDevice(pDxgiAdapter1, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(res))
			{
				if (pDxgiAdapter4 != nullptr)
					pDxgiAdapter4->Release();

				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				res = pDxgiAdapter1->QueryInterface(&pDxgiAdapter4);
				ThrowIfFailed(res);
			}
			
			pDxgiAdapter1->Release();
			
		}
	}

	pDxgiFactory->Release();

	return pDxgiAdapter4;
}

void RenderModule::EnableDebugLayer()
{
#if defined(_DEBUG)
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_pDebugInterface)));
	m_pDebugInterface->EnableDebugLayer();
#endif
}

void RenderModule::CheckTearingSupport()
{
	m_allowTearing = true;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
	// graphics debugging tools which will not support the 1.5 factory interface 
	// until a future update.
	IDXGIFactory4* pFactory4;
	HRESULT res = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory4));
	if (SUCCEEDED(res))
	{
		IDXGIFactory5* pFactory5;
		if (SUCCEEDED(pFactory4->QueryInterface(&pFactory5)))
		{
			res = pFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &m_allowTearing, sizeof(m_allowTearing));
			if (FAILED(res))
			{
				m_allowTearing = false;
			}

			pFactory5->Release();
		}

		pFactory4->Release();
	}
}

void RenderModule::CreateSwapChain(HWND hWnd, ID3D12CommandQueue* pCommandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
{
	IDXGIFactory4* pDxgiFactory4;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT res = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&pDxgiFactory4));
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
	swapChainDesc.Flags = m_allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	IDXGISwapChain1* pSwapChain1;
	res = pDxgiFactory4->CreateSwapChainForHwnd(pCommandQueue, hWnd, &swapChainDesc, nullptr, nullptr, &pSwapChain1);
	ThrowIfFailed(res);

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	ThrowIfFailed(pDxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

	res = pSwapChain1->QueryInterface(&m_pSwapChain);
	ThrowIfFailed(res);

	pSwapChain1->Release();
	pDxgiFactory4->Release();
}

void RenderModule::CreateRTVDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pRTVDescriptorHeap)));
}

void RenderModule::CreateDSVDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pDSVDescriptorHeap)));
}

void RenderModule::UpdateRenderTargetViews()
{
	//UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int ii = 0; ii < m_numFrames; ++ii)
	{
		ID3D12Resource* backBuffer;
		ThrowIfFailed(m_pSwapChain->GetBuffer(ii, IID_PPV_ARGS(&backBuffer)));

		m_pDevice->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);

		m_pBackBuffers[ii] = backBuffer;

		rtvHandle.Offset(m_RTVDescriptorSize);
	}
}
