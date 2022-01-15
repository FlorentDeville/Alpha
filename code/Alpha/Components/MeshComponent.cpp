/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Components/MeshComponent.h"

#include "DirectXMath.h"

#include "Entities/LocatorEntity.h"
#include "GameMgr.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

MeshComponent::MeshComponent(GameEntity* pParent, RenderableId id)
	: GameComponent(pParent)
	, m_renderableId(id)
{}

MeshComponent::~MeshComponent()
{}

void MeshComponent::Render()
{
	LocatorEntity* pLocatorParent = static_cast<LocatorEntity*>(m_pParent);
	const DirectX::XMMATRIX& transform = pLocatorParent->GetTransform();

	//retrieve the camera position
	GameMgr& gameMgr = GameMgr::Get();

	const DirectX::XMMATRIX& view = gameMgr.GetView();
	const DirectX::XMMATRIX& projection = gameMgr.GetProjection();

	//compute the wvp
	DirectX::XMMATRIX wvp = DirectX::XMMatrixMultiply(transform, view);
	wvp = DirectX::XMMatrixMultiply(wvp, projection);

	Renderable* pRenderable = g_pRenderableMgr->GetRenderable(m_renderableId);
	RenderModule::Get().Render(*pRenderable, wvp);
}
