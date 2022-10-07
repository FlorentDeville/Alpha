/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Components/MeshComponent.h"

#include "DirectXMath.h"

#include "Entities/LocatorEntity.h"
#include "GameMgr.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

MeshComponent::MeshComponent(GameEntity* pParent, Rendering::MeshId meshId, Rendering::MaterialId materialId)
	: GameComponent(pParent)
	, m_meshId(meshId)
	, m_materialId(materialId)
{}

MeshComponent::~MeshComponent()
{}

void MeshComponent::Render()
{
	RenderModule& renderer = RenderModule::Get();

	LocatorEntity* pLocatorParent = static_cast<LocatorEntity*>(m_pParent);
	const DirectX::XMMATRIX& transform = pLocatorParent->GetTransform();

	//retrieve the camera position
	GameMgr& gameMgr = GameMgr::Get();

	const DirectX::XMMATRIX& view = gameMgr.GetView();
	const DirectX::XMMATRIX& projection = gameMgr.GetProjection();

	//compute the wvp
	DirectX::XMMATRIX wvp = DirectX::XMMatrixMultiply(transform, view);
	wvp = DirectX::XMMatrixMultiply(wvp, projection);

	const Rendering::Material* pMaterial = Rendering::MaterialMgr::Get().GetMaterial(m_materialId);
	renderer.BindMaterial(*pMaterial, wvp);

	const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(m_meshId);
	renderer.RenderMesh(*pMesh);
}
