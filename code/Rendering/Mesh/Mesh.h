/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <d3d12.h>
#include <string>

#include "Core/Math/Vectors.h"

namespace Rendering
{
	struct VertexPos
	{
		Core::Float3 Position;
	};

	struct VertexPosUv
	{
		Core::Float3 Position;
		Core::Float2 Uv;
	};

	struct VertexPosColor
	{
		Core::Float3 Position;
		Core::Float3 Color;
	};

	struct VertexGeneric
	{
		Core::Float3 Position;
		Core::Float3 Color;
		Core::Float2 Uv;
		Core::Float3 Normal;
	};

	struct VertexText
	{
		Core::Float4		Position;		// (x, y) : top left position of the quad, (z, w) : width, height of the quad
		Core::Float4		Uv;				// (x, y) uv of the top left of the character in the texture, (z, w) width, height of the character in the texture
		Core::Float4		Color;			// color of the character
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
		void LoadVertexAndIndexBuffer(const VertexGeneric* pVertices, int verticesCount, const uint16_t* pIndices, int indicesCount);

		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const;
		const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

		int GetIndicesCount() const;
		int GetVerticesCount() const;

	private:
		//Vertex buffer
		ID3D12Resource* m_pVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		//Index buffer
		ID3D12Resource* m_pIndexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

		int m_indicesCount;
		int m_verticesCount;
	};
}
