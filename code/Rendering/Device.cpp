/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Device.h"

#include "Core/Helper.h"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace Rendering
{
	class DeviceImpl
	{
	public:
		ID3D12Device2* m_pDevice;
	};

	IDXGIAdapter4* GetAdapter(bool useWarp)
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

	Device::Device()
	{
		m_pImpl = new DeviceImpl();
	}

	Device::~Device()
	{
		delete m_pImpl;
		m_pImpl = nullptr;
	}

	void Device::Init()
	{
		IDXGIAdapter4* pAdapter = GetAdapter(false);

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

		m_pImpl->m_pDevice = pD3d12Device2;

		pAdapter->Release();
	}

	void Device::Shutdown()
	{
		m_pImpl->m_pDevice->Release();
	}

	ID3D12Device2* Device::GetDx12Device()
	{
		return m_pImpl->m_pDevice;
	}
}
