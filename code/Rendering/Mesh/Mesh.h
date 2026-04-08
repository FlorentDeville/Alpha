/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vectors.h"

#include "Rendering/BufferHandle.h"
#include "Rendering/ResourceFormat.h"

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

		uint64_t GetVertexBufferStartAddr() const;
		uint64_t GetVertexBufferOffset() const;
		uint32_t GetVertexBufferSize() const;
		uint32_t GetVertexBufferStride() const;

		uint64_t GetIndexBufferStartAddr() const;
		uint64_t GetIndexBufferOffset() const;
		uint32_t GetIndexBufferSize() const;
		ResourceFormat GetIndexBufferFormat() const;

		int GetIndicesCount() const;
		int GetVerticesCount() const;

	private:
		//Vertex buffer
		BufferHandle m_vertexBuffer;
		
		uint64_t m_vertexBufferStartAddr;
		uint64_t m_vertexBufferOffset;
		uint32_t m_vertexBufferSize;
		uint32_t m_vertexBufferStride;

		//Index buffer
		BufferHandle m_indexBuffer;
		uint64_t m_indexBufferStartAddr;
		uint64_t m_indexBufferOffset;
		uint32_t m_indexBufferSize;
		ResourceFormat m_indexBufferFormat;

		int m_indicesCount;
		int m_verticesCount;
	};
}
