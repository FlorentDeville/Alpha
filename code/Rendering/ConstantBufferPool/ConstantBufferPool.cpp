/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/ConstantBufferPool/ConstantBufferPool.h"

#include "Core/Helper.h"
#include "Rendering/RenderModule.h"

#include <assert.h>
#include <d3dx12.h>

namespace Rendering
{
	ConstantBufferPool::ConstantBufferPool()
		: m_pResource(nullptr)
		, m_pDescriptorHeap(nullptr)
		, m_elementSize(0)
		, m_elementCount(0)
		, m_current(0)
		, m_pResourcePtr(nullptr)
	{ }

	ConstantBufferPool::~ConstantBufferPool()
	{
		if (m_pResource)
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}

		if (m_pDescriptorHeap)
		{
			m_pDescriptorHeap->Release();
			m_pDescriptorHeap = nullptr;
		}
	}

	void ConstantBufferPool::Init(int elementSize, int elementCount)
	{
		assert(elementSize % 256 == 0); //must be aligned to 256 bytes

		m_elementSize = elementSize;
		m_elementCount = elementCount;

		ID3D12Device* pDevice = RenderModule::Get().GetDevice();

		//allocate the resource (buffer)
		uint64_t bufferSize = elementSize * elementCount;

		D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

		D3D12_HEAP_PROPERTIES heapProrperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);
		HRESULT res = pDevice->CreateCommittedResource(&heapProrperty, D3D12_HEAP_FLAG_NONE, &resourceDesc,
			resourceState,
			nullptr,
			IID_PPV_ARGS(&m_pResource));

		ThrowIfFailed(res);

		//get the cpu pointer to the resource buffer.
		CD3DX12_RANGE readRange(0, 0); // I don't read from the cpu, I only write
		res = m_pResource->Map(0, &readRange, reinterpret_cast<void**>(&m_pResourcePtr)); //Apparently I can leave the resource mapped.
		ThrowIfFailed(res);

		//create a descriptor heap to store the descriptor of each individual constant buffer
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = elementCount;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		res = pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pDescriptorHeap));

		ThrowIfFailed(res);

		UINT descriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//now create the constant buffer view (cbv) for each constant buffer
		for (int ii = 0; ii < elementCount; ++ii)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.SizeInBytes = elementSize;
			cbvDesc.BufferLocation = m_pResource->GetGPUVirtualAddress() + (ii * elementSize);

			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandle(m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), ii, descriptorSize);

			pDevice->CreateConstantBufferView(&cbvDesc, cpuDescHandle);
		}
	}

	void ConstantBufferPool::PreRender()
	{
		m_current = 0;
	}

	void ConstantBufferPool::PostRender()
	{

	}

	int ConstantBufferPool::GetFreeConstantBufferIndex()
	{
		assert(m_current < m_elementCount);

		int ret = m_current;
		++m_current;
		return ret;
	}

	D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferPool::GetGpuAddress(int poolIndex)
	{
		return m_pResource->GetGPUVirtualAddress() + (poolIndex * m_elementSize);
	}

	void ConstantBufferPool::Copy(int poolIndex, const void* pData, uint64_t size)
	{
		char* pDest = m_pResourcePtr + m_elementSize * poolIndex;

		memcpy(pDest, pData, size);
	}

	void ConstantBufferPool::Copy(int poolIndex, uint64_t offset, const void* pData, uint64_t size)
	{
		char* pDest = m_pResourcePtr + (m_elementSize * poolIndex) + offset;

		memcpy(pDest, pData, size);
	}
}
