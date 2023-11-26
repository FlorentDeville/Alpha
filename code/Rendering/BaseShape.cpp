/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/BaseShape.h"

#include "Rendering/Mesh/Mesh.h"

#include <cmath>
#include <vector>

//#pragma optimize("", off)

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

    DirectX::XMVECTOR GetCircleTangent(size_t i, size_t tessellation) noexcept
    {
        const float angle = (float(i) * DirectX::XM_2PI / float(tessellation)) + DirectX::XM_PIDIV2;
        float dx, dz;

        DirectX::XMScalarSinCos(&dx, &dz, angle);

        const DirectX::XMVECTORF32 v = { { { dx, 0, dz, 0 } } };
        return v;
    }

    // Helper creates a triangle fan to close the end of a cylinder / cone
    void CreateCylinderCap(std::vector<Rendering::VertexPosColor>& vertices, std::vector<uint16_t>& indices, size_t tessellation, float height, float radius, bool isTop)
    {
        // Create cap indices.
        for (uint16_t i = 0; i < tessellation - 2; i++)
        {
            uint16_t i1 = (i + 1) % tessellation;
            uint16_t i2 = (i + 2) % tessellation;

            if (isTop)
            {
                std::swap(i1, i2);
            }

            const uint16_t vbase = static_cast<uint16_t>(vertices.size());
            indices.push_back(vbase);
            indices.push_back(vbase + i2);
            indices.push_back(vbase + i1);
        }

        // Which end of the cylinder is this?
        DirectX::XMVECTOR normal = DirectX::g_XMIdentityR1;
        DirectX::XMVECTOR textureScale = DirectX::g_XMNegativeOneHalf;

        if (!isTop)
        {
            normal = DirectX::XMVectorNegate(normal);
            textureScale = DirectX::XMVectorMultiply(textureScale, DirectX::g_XMNegateX);
        }

        // Create cap vertices.
        for (size_t i = 0; i < tessellation; i++)
        {
            const DirectX::XMVECTOR circleVector = GetCircleVector(i, tessellation);

            const DirectX::XMVECTOR position = DirectX::XMVectorAdd(DirectX::XMVectorScale(circleVector, radius), DirectX::XMVectorScale(normal, height));

            const DirectX::XMVECTOR textureCoordinate = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorSwizzle<0, 2, 3, 3>(circleVector), textureScale, DirectX::g_XMOneHalf);

            vertices.push_back(Rendering::VertexPosColor());
            Rendering::VertexPosColor& vertex = vertices.back();
            vertex.Position = DirectX::XMFLOAT3(position.m128_f32[0], position.m128_f32[1], position.m128_f32[2]);
            vertex.Color = DirectX::XMFLOAT3(1, 0, 0);
        }
    }

	void BaseShape::CreateCircle(Mesh* pMesh, int resolution)
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

	void BaseShape::CreateCylinder(Mesh* pMesh, float height, float diameter, int tessellation)
	{
        if (tessellation < 3)
            return;

        std::vector<Rendering::VertexPosColor> vertices;
        std::vector<uint16_t> indices;

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

            {
                vertices.push_back(Rendering::VertexPosColor());
                Rendering::VertexPosColor& vertex = vertices.back();

                DirectX::XMVECTOR pos = DirectX::XMVectorAdd(sideOffset, topOffset);
                vertex.Position = DirectX::XMFLOAT3(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
                vertex.Color = DirectX::XMFLOAT3(1, 0, 0);
            }

            {
                vertices.push_back(Rendering::VertexPosColor());
                Rendering::VertexPosColor& vertex = vertices.back();

                DirectX::XMVECTOR pos = DirectX::XMVectorSubtract(sideOffset, topOffset);
                vertex.Position = DirectX::XMFLOAT3(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
                vertex.Color = DirectX::XMFLOAT3(1, 0, 0);
            }

            indices.push_back(i * 2);
            indices.push_back(i * 2 + 1);
            indices.push_back((i * 2 + 2) % (stride * 2));

            indices.push_back(i * 2 + 1);
            indices.push_back((i * 2 + 3) % (stride * 2));
            indices.push_back((i * 2 + 2) % (stride * 2));
        }

        // Create flat triangle fan caps to seal the top and bottom.
        CreateCylinderCap(vertices, indices, tessellation, height, radius, true);
        CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

        pMesh->LoadVertexAndIndexBuffer(vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
	}

    void BaseShape::CreateCone(Mesh* pMesh, float diameter, float height, size_t tessellation)
    {
        std::vector<Rendering::VertexPosColor> vertices;
        std::vector<uint16_t> indices;

        height /= 2;

        const DirectX::XMVECTOR topOffset = DirectX::XMVectorScale(DirectX::g_XMIdentityR1, height);

        const float radius = diameter / 2;
        const size_t stride = tessellation + 1;

        // Create a ring of triangles around the outside of the cone.
        for (uint16_t i = 0; i <= tessellation; i++)
        {
            const DirectX::XMVECTOR circlevec = GetCircleVector(i, tessellation);

            const DirectX::XMVECTOR sideOffset = DirectX::XMVectorScale(circlevec, radius);

            //const float u = float(i) / float(tessellation);

            //const DirectX::XMVECTOR textureCoordinate = DirectX::XMLoadFloat(&u);

            const DirectX::XMVECTOR pt = DirectX::XMVectorSubtract(sideOffset, topOffset);

           /* DirectX::XMVECTOR normal = DirectX::XMVector3Cross(
                GetCircleTangent(i, tessellation),
                DirectX::XMVectorSubtract(topOffset, pt));
            normal = DirectX::XMVector3Normalize(normal);*/

            // Duplicate the top vertex for distinct normals
            Rendering::VertexPosColor v1;
            v1.Position = DirectX::XMFLOAT3(topOffset.m128_f32[0], topOffset.m128_f32[1], topOffset.m128_f32[2]);
            vertices.push_back(v1);

            Rendering::VertexPosColor v2;
            v2.Position = DirectX::XMFLOAT3(pt.m128_f32[0], pt.m128_f32[1], pt.m128_f32[2]);
            vertices.push_back(v2);

            indices.push_back(i * 2);
            indices.push_back((i * 2 + 1) % (stride * 2));
            indices.push_back((i * 2 + 3) % (stride * 2));
        }

        // Create flat triangle fan caps to seal the bottom.
        CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

        pMesh->LoadVertexAndIndexBuffer(vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
    }

    void BaseShape::CreateTorus(Mesh* pMesh, float diameter, float thickness, int tessellation)
    {
        std::vector<VertexPosColor> vertices;
        std::vector<uint16_t> indices;

        const int stride = tessellation + 1;

        // First we loop around the main ring of the torus.
        for (int ii = 0; ii <= tessellation; ii++)
        {
            const float u = float(ii) / float(tessellation);

            const float outerAngle = float(ii) * DirectX::XM_2PI / float(tessellation) - DirectX::XM_PIDIV2;

            // Create a transform matrix that will align geometry to
            // slice perpendicularly though the current ring position.
            const DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(diameter / 2, 0, 0) * DirectX::XMMatrixRotationY(outerAngle);

            // Now we loop along the other axis, around the side of the tube.
            for (int j = 0; j <= tessellation; j++)
            {
                const float v = 1 - float(j) / float(tessellation);

                const float innerAngle = float(j) * DirectX::XM_2PI / float(tessellation) + DirectX::XM_PI;
                float dx, dy;

                DirectX::XMScalarSinCos(&dy, &dx, innerAngle);

                // Create a vertex.
                DirectX::XMVECTOR normal = DirectX::XMVectorSet(dx, dy, 0, 0);
                DirectX::XMVECTOR position = DirectX::XMVectorScale(normal, thickness / 2);
                const DirectX::XMVECTOR textureCoordinate = DirectX::XMVectorSet(u, v, 0, 0);

                position = XMVector3Transform(position, transform);
                normal = XMVector3TransformNormal(normal, transform);

                VertexPosColor vertex;
                vertex.Position = DirectX::XMFLOAT3(position.m128_f32[0], position.m128_f32[1], position.m128_f32[2]);
                vertex.Color = DirectX::XMFLOAT3(0, 0, 0);
                vertices.push_back(vertex);

                // And create indices for two triangles.
                const int nextI = (ii + 1) % stride;
                const int nextJ = (j + 1) % stride;

                indices.push_back(ii * stride + j);
                indices.push_back(nextI * stride + j);
                indices.push_back(ii * stride + nextJ);

                indices.push_back(ii * stride + nextJ);
                indices.push_back(nextI * stride + j);
                indices.push_back(nextI * stride + nextJ);
            }
        }

        pMesh->LoadVertexAndIndexBuffer(vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
    }
}
