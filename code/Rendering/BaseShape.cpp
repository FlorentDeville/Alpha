/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/BaseShape.h"

#include "Rendering/Mesh/Mesh.h"

#include <cmath>

namespace Rendering
{
    // Helper computes a point on a unit circle, aligned to the x/z plane and centered on the origin.
    DirectX::XMVECTOR GetCircleVector(size_t i, size_t tessellation) noexcept
    {
        const float angle = float(i) * DirectX::XM_2PI / float(tessellation);
        float dx, dz;

        DirectX::XMScalarSinCos(&dx, &dz, angle);

        const DirectX::XMVECTORF32 v = { { { dx, 0, dz, 0 } } };
        return v;
    }

	void BaseShape::CreateCircle(Rendering::Mesh* pMesh, int resolution)
	{
		const int VERTEX_COUNT = resolution + 1;
		const float THETA = 2 * 3.14f / resolution;
		Rendering::VertexPosColor* pVertices = new Rendering::VertexPosColor[VERTEX_COUNT];
		for (int ii = 0; ii < VERTEX_COUNT; ++ii)
		{
			float currentTheta = ii * THETA;
			float x = std::cos(currentTheta);
			float z = std::sin(currentTheta);

			pVertices[ii].Position = DirectX::XMFLOAT3(x, 0, z);
			pVertices[ii].Color = DirectX::XMFLOAT3(1, 0, 0);
		}

		pMesh->LoadVertexAndIndexBuffer(pVertices, VERTEX_COUNT, nullptr, 0);

		delete[] pVertices;
	}

	void BaseShape::CreateCylinder(Rendering::Mesh* pMesh, float height, float diameter, int tessellation)
	{
        if (tessellation < 3)
            return;

        int vertexCount = (tessellation + 1) * 2;
        Rendering::VertexPosColor* pVertices = new Rendering::VertexPosColor[vertexCount];
        int vertexSize = 0;

        int indicesCount = (tessellation + 1) * 6;
        uint16_t* pIndices = new uint16_t[indicesCount];
        int indicesSize = 0;

        height /= 2;

        const DirectX::XMVECTOR topOffset = DirectX::XMVectorScale(DirectX::g_XMIdentityR1, height);

        const float radius = diameter / 2;
        const size_t stride = tessellation + 1;

        // Create a ring of triangles around the outside of the cylinder.
        for (uint16_t i = 0; i <= tessellation; i++)
        {
            const DirectX::XMVECTOR normal = GetCircleVector(i, tessellation);

            const DirectX::XMVECTOR sideOffset = DirectX::XMVectorScale(normal, radius);

            const float u = float(i) / float(tessellation);

            const DirectX::XMVECTOR textureCoordinate = DirectX::XMLoadFloat(&u);

            DirectX::XMVECTOR pos = DirectX::XMVectorAdd(sideOffset, topOffset);
            pVertices[vertexSize].Position = DirectX::XMFLOAT3(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
            pVertices[vertexSize].Color = DirectX::XMFLOAT3(1, 0, 0);
            ++vertexSize;

            pos = DirectX::XMVectorSubtract(sideOffset, topOffset);
            pVertices[vertexSize].Position = DirectX::XMFLOAT3(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
            pVertices[vertexSize].Color = DirectX::XMFLOAT3(1, 0, 0);
            ++vertexSize;
            
            pIndices[indicesSize++] = i * 2;
            pIndices[indicesSize++] = i * 2 + 1;
            pIndices[indicesSize++] = (i * 2 + 2) % (stride * 2);

            pIndices[indicesSize++] = i * 2 + 1;
            pIndices[indicesSize++] = (i * 2 + 3) % (stride * 2);
            pIndices[indicesSize++] = (i * 2 + 2) % (stride * 2);

            //index_push_back(indices, i * 2);
            //index_push_back(indices, (i * 2 + 2) % (stride * 2));
            //index_push_back(indices, i * 2 + 1);

            //index_push_back(indices, i * 2 + 1);
            //index_push_back(indices, (i * 2 + 2) % (stride * 2));
            //index_push_back(indices, (i * 2 + 3) % (stride * 2));
        }

        // Create flat triangle fan caps to seal the top and bottom.
        //CreateCylinderCap(vertices, indices, tessellation, height, radius, true);
        //CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

        pMesh->LoadVertexAndIndexBuffer(pVertices, vertexCount, pIndices, indicesCount);

        delete[] pVertices;
        delete[] pIndices;
	}
}
