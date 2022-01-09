/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <d3d12.h>
#include <cstdint>

namespace Rendering
{
	class DescriptorHeap
	{
	public:
		DescriptorHeap();
		~DescriptorHeap();

		void Init(ID3D12Device2* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
		void Release();

		D3D12_CPU_DESCRIPTOR_HANDLE GetNewHandle();

	private:
		D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
		uint32_t m_count;
		uint32_t m_current;
		ID3D12DescriptorHeap* m_pHeap;
		uint32_t m_descriptorSize;
	};
}
