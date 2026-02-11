/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/BaseShape.h"

#include "Core/Collections/Array.h"
#include "Core/Math/Constants.h"

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

    void BaseShape::CreateCube(Mesh* pMesh)
    {
        const float HALF_SIZE = 0.5f;
        const int VERTEX_COUNT = 8;
        Rendering::VertexPosColor vertices[VERTEX_COUNT];
        vertices[0].Position = DirectX::XMFLOAT3(HALF_SIZE, HALF_SIZE, HALF_SIZE);
        vertices[1].Position = DirectX::XMFLOAT3(HALF_SIZE, HALF_SIZE, -HALF_SIZE);
        vertices[2].Position = DirectX::XMFLOAT3(-HALF_SIZE, HALF_SIZE, -HALF_SIZE);
        vertices[3].Position = DirectX::XMFLOAT3(-HALF_SIZE, HALF_SIZE, HALF_SIZE);

        vertices[4].Position = DirectX::XMFLOAT3(HALF_SIZE, -HALF_SIZE, HALF_SIZE);
        vertices[5].Position = DirectX::XMFLOAT3(HALF_SIZE, -HALF_SIZE, -HALF_SIZE);
        vertices[6].Position = DirectX::XMFLOAT3(-HALF_SIZE, -HALF_SIZE, -HALF_SIZE);
        vertices[7].Position = DirectX::XMFLOAT3(-HALF_SIZE, -HALF_SIZE, HALF_SIZE);

        const int INDEX_COUNT = 36;
        //const int INDEX_COUNT = 6;
        uint16_t indices[INDEX_COUNT];

        //top face
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        indices[3] = 0;
        indices[4] = 2;
        indices[5] = 3;

        //bottom face
        indices[6] = 5;
        indices[7] = 4;
        indices[8] = 7;

        indices[9] = 6;
        indices[10] = 5;
        indices[11] = 7;

        //front
        indices[12] = 2;
        indices[13] = 1;
        indices[14] = 5;

        indices[15] = 2;
        indices[16] = 5;
        indices[17] = 6;

        //back
        indices[18] = 0;
        indices[19] = 3;
        indices[20] = 7;

        indices[21] = 0;
        indices[22] = 7;
        indices[23] = 4;

        //left
        indices[24] = 2;
        indices[25] = 6;
        indices[26] = 7;

        indices[27] = 2;
        indices[28] = 7;
        indices[29] = 3;

        //right
        indices[30] = 1;
        indices[31] = 0;
        indices[32] = 4;

        indices[33] = 1;
        indices[34] = 4;
        indices[35] = 5;

        pMesh->LoadVertexAndIndexBuffer(vertices, VERTEX_COUNT, indices, INDEX_COUNT);
    }

    void BaseShape::CreateSphere(Mesh* pMesh, uint32_t stacks, uint32_t slices)
    {
        Core::Array<VertexPosColor> vertices;
        Core::Array<uint16_t> indices;

        float fStacks = static_cast<float>(stacks);
        float fSlices = static_cast<float>(slices);

        //go up and down along the sphere, or polar angle. The angle starts from the axis between the poles, not the equator
        for (uint32_t ii = 0; ii < stacks; ii++)
        {
            float theta1 = (ii / fStacks) * Core::PI;
            float theta2 = ((ii + 1) / fStacks) * Core::PI;

            float cosTheta1 = cos(theta1);
            float sinTheta1 = sin(theta1);

            float cosTheta2 = cos(theta2);
            float sinTheta2 = sin(theta2);

            for (uint32_t jj = 0; jj < slices; jj++) // go around the sphere, or azimut angle
            {
                float phi1 = (jj / fSlices) * Core::TWO_PI;
                float phi2 = ((jj + 1) / fSlices) * Core::TWO_PI;

                float cosPhi1 = cos(phi1);
                float sinPhi1 = sin(phi1);

                float cosPhi2 = cos(phi2);
                float sinPhi2 = sin(phi2);

                //phi2   phi1
                // |      |
                // 2------1 -- theta1
                // |\ _   |
                // |    \ |
                // 3------4 -- theta2
                //

                //convert polar coordinates to cartesian coordinates
                VertexPosColor vertex1;
                vertex1.Position.x = sinTheta1 * cosPhi1;
                vertex1.Position.y = sinTheta1 * sinPhi1;
                vertex1.Position.z = cosTheta1;
                vertex1.Color = DirectX::XMFLOAT3(0, 0, 0);

                VertexPosColor vertex2;
                vertex2.Position.x = sinTheta1 * cosPhi2;
                vertex2.Position.y = sinTheta1 * sinPhi2;
                vertex2.Position.z = cosTheta1;
                vertex2.Color = DirectX::XMFLOAT3(0, 0, 0);

                VertexPosColor vertex3;
                vertex3.Position.x = sinTheta2 * cosPhi2;
                vertex3.Position.y = sinTheta2 * sinPhi2;
                vertex3.Position.z = cosTheta2;
                vertex3.Color = DirectX::XMFLOAT3(0, 0, 0);

                VertexPosColor vertex4;
                vertex4.Position.x = sinTheta2 * cosPhi1;
                vertex4.Position.y = sinTheta2 * sinPhi1;
                vertex4.Position.z = cosTheta2;
                vertex4.Color = DirectX::XMFLOAT3(0, 0, 0);

                int indexVertex1 = vertices.GetSize();
                int indexVertex2 = indexVertex1 + 1;
                int indexVertex3 = indexVertex2 + 1;
                int indexVertex4 = indexVertex3 + 1;

                vertices.PushBack(vertex1);
                vertices.PushBack(vertex2);
                vertices.PushBack(vertex3);
                vertices.PushBack(vertex4);

                // facing out
                if (ii == 0) // top cap
                {
                    indices.PushBack(indexVertex1);
                    indices.PushBack(indexVertex3);
                    indices.PushBack(indexVertex4);
                }
                else if (ii + 1 == stacks) //end cap
                {
                    indices.PushBack(indexVertex3);
                    indices.PushBack(indexVertex1);
                    indices.PushBack(indexVertex2);
                }
                else
                {
                    indices.PushBack(indexVertex1);
                    indices.PushBack(indexVertex2);
                    indices.PushBack(indexVertex4);

                    indices.PushBack(indexVertex2);
                    indices.PushBack(indexVertex3);
                    indices.PushBack(indexVertex4);
                }
            }
        }

        pMesh->LoadVertexAndIndexBuffer(vertices.GetData(), vertices.GetSize(), indices.GetData(), indices.GetSize());
    }
}
