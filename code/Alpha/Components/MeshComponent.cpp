/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Components/MeshComponent.h"

#include "Entities/LocatorEntity.h"
#include "DirectXMath.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

MeshComponent::MeshComponent(GameEntity* pParent)
	: GameComponent(pParent)
{}

MeshComponent::~MeshComponent()
{}

void MeshComponent::Render()
{
	LocatorEntity* pLocatorParent = static_cast<LocatorEntity*>(m_pParent);
	const DirectX::XMMATRIX& transform = pLocatorParent->GetTransform();

	//retrieve the camera position

	//compute the projection

	//compute the wvp

	Renderable* pRenderable = g_pRenderableMgr->GetRenderable(m_renderableId);
	RenderModule::Get().Render(*pRenderable, transform);
}
