/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/DescriptorHeap.h"

#include "Helper.h"

#include <d3dx12.h>

namespace Rendering
{
	DescriptorHeap::DescriptorHeap()
		: m_heapType()
		, m_count(0)
		, m_current(0)
		, m_pHeap(nullptr)
		, m_descriptorSize(0)
	{}

	DescriptorHeap::~DescriptorHeap()
	{}

	void DescriptorHeap::Init(ID3D12Device2* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
	{
		m_heapType = type;
		m_count = numDescriptors;

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = numDescriptors;
		dsvHeapDesc.Type = type;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pHeap)));

		m_descriptorSize = pDevice->GetDescriptorHandleIncrementSize(type);
	}

	void DescriptorHeap::Release()
	{
		if (m_pHeap)
		{
			m_pHeap->Release();
			m_pHeap = nullptr;
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetNewHandle()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_pHeap->GetCPUDescriptorHandleForHeapStart(), m_current, m_descriptorSize);
		++m_current;
		return rtv;
	}
}
