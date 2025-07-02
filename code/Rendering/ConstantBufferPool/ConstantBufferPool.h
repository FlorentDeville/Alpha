/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct D3D12_CPU_DESCRIPTOR_HANDLE;

#include <d3d12.h>
#include <vector>

namespace Rendering
{
	//This is a dynamic constant buffer pool. Dynamic means the buffer can be updated every frame.
	class ConstantBufferPool
	{
	public:
		ConstantBufferPool();
		virtual ~ConstantBufferPool();

		virtual void Init(int elementSize, int elementCount);

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress(int poolIndex);

		//Copy pData into the constant buffer at index poolIndex
		void Copy(int poolIndex, const void* pData, uint64_t size);

		//Copy pData in the constant buffer at index poolIndex. offset is the starting byte inside the constant buffer
		void Copy(int poolIndex, uint64_t offset, const void* pData, uint64_t size);

	protected:
		//Size of a single constant buffer
		int m_elementSize;

		//Number of constant buffer in the pool
		int m_elementCount;

		//Giant buffer subdivided in smaller constant buffer
		ID3D12Resource* m_pResource;

		//Heap containing the descriptor of each individual constant buffer
		ID3D12DescriptorHeap* m_pDescriptorHeap;

		//CPU pointer to the resource
		char* m_pResourcePtr;
	};
}
