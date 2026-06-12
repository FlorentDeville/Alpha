/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/RenderableComponent.h"

namespace Systems
{
	RenderableComponent::RenderableComponent()
		: GameComponent()
		, m_pMesh(nullptr)
		, m_pMaterial(nullptr)
		, m_mesh()
		, m_material()
		, m_lsTx()
	{ }

	RenderableComponent::~RenderableComponent()
	{ }

	void RenderableComponent::OnStart(GameContext* pWorld)
	{
		m_pMesh = m_mesh.GetPtr();
		m_pMaterial = m_material.GetPtr();
	}

	void RenderableComponent::OnDestroy(GameContext* pWorld)
	{
		m_pMesh = nullptr;
		m_pMaterial = nullptr;
	}

	void RenderableComponent::SetMesh(MeshAsset* pMesh)
	{
		m_pMesh = pMesh;
	}

	const MeshAsset* RenderableComponent::GetMesh() const
	{
		return m_pMesh;
	}

	const Core::Sqt& RenderableComponent::GetLocalTx() const
	{
		return m_lsTx;
	}

	void RenderableComponent::SetLocalRotation(const Core::Quaternion& rotation)
	{
		m_lsTx.SetRotationQuaternion(rotation);
	}

	void RenderableComponent::SetMaterialInstance(MaterialInstanceAsset* pMaterial)
	{
		m_pMaterial = pMaterial;
	}

	const MaterialInstanceAsset* RenderableComponent::GetMaterialInstance() const
	{
		return m_pMaterial;
	}
}
