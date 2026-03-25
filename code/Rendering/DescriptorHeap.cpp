/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/DescriptorHeap.h"

#include "Core/Helper.h"

#include "Rendering/Device.h"

#include <assert.h>
#include <d3dx12.h>

namespace Rendering
{
	class DescriptorHeapImpl
	{
	public:
		uint32_t m_current;
		ID3D12DescriptorHeap* m_pHeap;
		uint32_t m_descriptorSize;

		DescriptorHeapImpl()
			: m_current(0)
			, m_pHeap(nullptr)
			, m_descriptorSize(0)
		{ }
	};

	DescriptorHeap::DescriptorHeap()
	{
		m_pImpl = new DescriptorHeapImpl();
	}

	DescriptorHeap::~DescriptorHeap()
	{
		delete m_pImpl;
		m_pImpl = nullptr;
	}

	void DescriptorHeap::Init(Device* pDevice, DescriptorHeapFlag flag, DescriptorHeapType type, uint32_t numDescriptors)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = numDescriptors;

		switch (type)
		{
		case DescriptorHeapType::CBV_SRV_UAV:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			break;

		case DescriptorHeapType::RTV:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			break;

		case DescriptorHeapType::DSV:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			break;

		case DescriptorHeapType::SAMPLER:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			break;

		default:
			assert(false && "Unknown descriptor heap type");
			break;
		}
		
		switch (flag)
		{
		case DescriptorHeapFlag::DEFAULT:
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			break;

		case DescriptorHeapFlag::SHADER_VISIBLE:
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			break;

		default:
			assert(false && "Unknown descriptor heap flag");
			break;
		}
		
		ThrowIfFailed(pDevice->GetDx12Device()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pImpl->m_pHeap)));

		m_pImpl->m_descriptorSize = pDevice->GetDx12Device()->GetDescriptorHandleIncrementSize(heapDesc.Type);
	}

	void DescriptorHeap::Release()
	{
		if (m_pImpl->m_pHeap)
		{
			m_pImpl->m_pHeap->Release();
			m_pImpl->m_pHeap = nullptr;
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetNewHandle()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_pImpl->m_pHeap->GetCPUDescriptorHandleForHeapStart(), m_pImpl->m_current, m_pImpl->m_descriptorSize);
		++(m_pImpl->m_current);
		return rtv;
	}
}
