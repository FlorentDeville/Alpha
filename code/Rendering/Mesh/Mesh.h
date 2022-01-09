/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

#include <d3d12.h>

struct VertexPos
{
	DirectX::XMFLOAT3 Position;
};

struct VertexPosUv
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Uv;
};

struct VertexPosColor
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;
};

struct VertexText
{
	DirectX::XMFLOAT4	Position;		// (x, y) : top left position of the quad, (z, w) : width, height of the quad
	DirectX::XMFLOAT4	Uv;				// (x, y) uv of the top left of the character in the texture, (z, w) width, height of the character in the texture
	DirectX::XMFLOAT4	Color;			// color of the character
	float				Z;				// Depth
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadVertexAndIndexBuffer(const VertexPos* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount);
	void LoadVertexAndIndexBuffer(const VertexPosColor* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount);
	void LoadVertexAndIndexBuffer(const VertexPosUv* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount);

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
