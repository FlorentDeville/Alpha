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
