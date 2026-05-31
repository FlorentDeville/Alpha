/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/MeshComponent.h"

#include "Rendering/Camera.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/RenderModule.h"

#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/MaterialRendering.h"

namespace Systems
{
	MeshComponent::MeshComponent()
		: GameComponent()
		, m_mesh()
		, m_material()
	{ }

	MeshComponent::~MeshComponent()
	{ }

	const MeshAsset* MeshComponent::GetMesh() const
	{
		return m_mesh.GetPtr();
	}

	const MaterialInstanceAsset* MeshComponent::GetMaterialInstance() const
	{
		return m_material.GetPtr();
	}
}
