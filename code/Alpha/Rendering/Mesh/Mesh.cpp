/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Mesh.h"

#include <d3dx12.h>

#include "Rendering/CommandQueue.h"
#include "Rendering/RenderModule.h"
#include "Core/Helper.h"

static void UpdateBufferResource(ID3D12GraphicsCommandList2* pCommandList, ID3D12Resource** pDestinationResource,
	ID3D12Resource** pIntermediateResource, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags)
{
	size_t bufferSize = numElements * elementSize;

	// Create a committed resource for the GPU resource in a default heap.
	D3D12_HEAP_PROPERTIES heapProrperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
	HRESULT res = RenderModule::Get().GetDevice()->CreateCommittedResource(&heapProrperty, D3D12_HEAP_FLAG_NONE, &resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pDestinationResource));

	ThrowIfFailed(res);

	// Create an committed resource for the upload.
	if (bufferData)
	{
		D3D12_HEAP_PROPERTIES heapPropertyInt = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDescInt = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		res = RenderModule::Get().GetDevice()->CreateCommittedResource(
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

Mesh::Mesh()
	: m_pVertexBuffer(nullptr)
	, m_vertexBufferView()
	, m_pIndexBuffer(nullptr)
	, m_indexBufferView()
	, m_indicesCount(0)
{}

Mesh::~Mesh()
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();
}

void Mesh::LoadVertexAndIndexBuffer(const VertexPos* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount)
{
	m_indicesCount = indicesCount;

	CommandQueue* pCopyCommandQueue = RenderModule::Get().GetCopyCommandQueue();
	ID3D12GraphicsCommandList2* pCommandList = pCopyCommandQueue->GetCommandList();

	ID3D12Resource* pIntermediateVertexBuffer;
	UpdateBufferResource(pCommandList, &m_pVertexBuffer, &pIntermediateVertexBuffer, verticesCount, sizeof(VertexPos), pVertices, D3D12_RESOURCE_FLAG_NONE);

	// Create the vertex buffer view.
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(VertexPos) * verticesCount;
	m_vertexBufferView.StrideInBytes = sizeof(VertexPos);

	ID3D12Resource* pIntermediateIndexBuffer;
	UpdateBufferResource(pCommandList, &m_pIndexBuffer, &pIntermediateIndexBuffer, indicesCount, sizeof(uint16_t), pIndices, D3D12_RESOURCE_FLAG_NONE);

	// Create index buffer view.
	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = sizeof(uint16_t) * indicesCount;

	//Upload everything to the gpu
	uint64_t fenceValue = pCopyCommandQueue->ExecuteCommandList(pCommandList);
	pCopyCommandQueue->WaitForFenceValue(fenceValue);
}

void Mesh::LoadVertexAndIndexBuffer(const VertexPosColor* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount)
{
	m_indicesCount = indicesCount;

	CommandQueue* pCopyCommandQueue = RenderModule::Get().GetCopyCommandQueue();
	ID3D12GraphicsCommandList2* pCommandList = pCopyCommandQueue->GetCommandList();

	ID3D12Resource* pIntermediateVertexBuffer;
	UpdateBufferResource(pCommandList, &m_pVertexBuffer, &pIntermediateVertexBuffer, verticesCount, sizeof(VertexPosColor), pVertices, D3D12_RESOURCE_FLAG_NONE);

	// Create the vertex buffer view.
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(VertexPosColor) * verticesCount;
	m_vertexBufferView.StrideInBytes = sizeof(VertexPosColor);

	ID3D12Resource* pIntermediateIndexBuffer;
	UpdateBufferResource(pCommandList, &m_pIndexBuffer, &pIntermediateIndexBuffer, indicesCount, sizeof(uint16_t), pIndices, D3D12_RESOURCE_FLAG_NONE);

	// Create index buffer view.
	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = sizeof(uint16_t) * indicesCount;

	//Upload everything to the gpu
	uint64_t fenceValue = pCopyCommandQueue->ExecuteCommandList(pCommandList);
	pCopyCommandQueue->WaitForFenceValue(fenceValue);
}

void Mesh::LoadVertexAndIndexBuffer(const VertexPosUv* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount)
{
	m_indicesCount = indicesCount;

	CommandQueue* pCopyCommandQueue = RenderModule::Get().GetCopyCommandQueue();
	ID3D12GraphicsCommandList2* pCommandList = pCopyCommandQueue->GetCommandList();

	ID3D12Resource* pIntermediateVertexBuffer;
	UpdateBufferResource(pCommandList, &m_pVertexBuffer, &pIntermediateVertexBuffer, verticesCount, sizeof(VertexPosUv), pVertices, D3D12_RESOURCE_FLAG_NONE);

	// Create the vertex buffer view.
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(VertexPosUv) * verticesCount;
	m_vertexBufferView.StrideInBytes = sizeof(VertexPosUv);

	ID3D12Resource* pIntermediateIndexBuffer;
	UpdateBufferResource(pCommandList, &m_pIndexBuffer, &pIntermediateIndexBuffer, indicesCount, sizeof(uint16_t), pIndices, D3D12_RESOURCE_FLAG_NONE);

	// Create index buffer view.
	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = sizeof(uint16_t) * indicesCount;

	//Upload everything to the gpu
	uint64_t fenceValue = pCopyCommandQueue->ExecuteCommandList(pCommandList);
	pCopyCommandQueue->WaitForFenceValue(fenceValue);
}

const D3D12_VERTEX_BUFFER_VIEW& Mesh::GetVertexBufferView() const
{
	return m_vertexBufferView;
}

const D3D12_INDEX_BUFFER_VIEW& Mesh::GetIndexBufferView() const
{
	return m_indexBufferView;
}

int Mesh::GetIndicesCount() const
{
	return m_indicesCount;
}
