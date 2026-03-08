/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

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
	{ }

	const MeshAsset* StaticMeshComponent::GetMesh() const
	{
		return m_mesh.GetPtr();
	}

	const MaterialInstanceAsset* StaticMeshComponent::GetMaterialInstance() const
	{
		return m_material.GetPtr();
	}
}
