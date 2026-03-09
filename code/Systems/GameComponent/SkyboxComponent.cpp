/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/SkyboxComponent.h"

namespace Systems
{
	SkyboxComponent::SkyboxComponent()
		: GameComponent()
		, m_mesh()
		, m_material()
	{ }

	SkyboxComponent::~SkyboxComponent()
	{ }

	const MeshAsset* SkyboxComponent::GetMesh() const
	{
		return m_mesh.GetPtr();
	}

	const MaterialInstanceAsset* SkyboxComponent::GetMaterialInstance() const
	{
		return m_material.GetPtr();
	}
}
