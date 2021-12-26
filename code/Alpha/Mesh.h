/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

#include <d3d12.h>

struct VertexPosColor
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadVertexAndIndexBuffer(const VertexPosColor* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const;
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

	int GetIndicesCount() const;

private:
	//Vertex buffer
	ID3D12Resource* m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	//Index buffer
	ID3D12Resource* m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	int m_indicesCount;
};
