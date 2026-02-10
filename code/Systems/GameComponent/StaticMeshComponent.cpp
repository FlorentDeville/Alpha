/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/StaticMeshComponent.h"

#include "Rendering/Camera.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/RenderModule.h"

#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/MaterialRendering.h"

namespace Systems
{
	StaticMeshComponent::StaticMeshComponent()
		: GameComponent()
		, m_mesh()
		, m_material()
	{ }

	StaticMeshComponent::~StaticMeshComponent()
	{ }

	void StaticMeshComponent::Render()
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		Rendering::Camera* pCamera = renderModule.GetCamera();

		const DirectX::XMMATRIX view = pCamera->GetViewMatrix();
		const DirectX::XMMATRIX proj = pCamera->GetProjectionMatrix();

		const Core::Mat44f& world = m_pOwnerGo->GetTransform().GetWorldTx();
		DirectX::XMMATRIX wvp = proj * (view * world.m_matrix);

		Systems::MeshAsset* pMesh = m_mesh.GetPtr();
		if (!pMesh)
			return;

		Systems::MaterialInstanceAsset* pMaterial = m_material.GetPtr();
		if (pMaterial && pMaterial->GetBaseMaterial() && pMaterial->GetBaseMaterial()->IsValidForRendering())
		{
			Rendering::PerObjectCBuffer perObjectData(world.m_matrix);

			DirectX::XMFLOAT3 cameraPosFloat3;
			DirectX::XMStoreFloat3(&cameraPosFloat3, DirectX::XMVectorNegate(view.r[3]));
			Rendering::PerFrameCBuffer perFrameData(view, proj, cameraPosFloat3);

			Rendering::LightsCBuffer lights;
			lights.AddLight()->MakeDirectionalLight(Core::Float3(0, -1, 0), Core::Float3(1, 1, 1), Core::Float3(1, 1, 1), Core::Float3(1, 1, 1));

			Systems::MaterialRendering::Bind(*pMaterial, perObjectData, perFrameData, lights);

			const Rendering::Mesh* pMesh = m_mesh->GetRenderingMesh();
			renderModule.RenderMesh(*pMesh);
		}
	}

	const MeshAsset* StaticMeshComponent::GetMesh() const
	{
		return m_mesh.GetPtr();
	}
}
