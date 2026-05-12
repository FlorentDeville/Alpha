/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/Collisions/CollisionSphereComponent.h"

#include "Systems/Game/Subsystems/Collision/CollisionSubsystem.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	CollisionSphereComponent::CollisionSphereComponent()
		: GameComponent()
		, m_center()
		, m_radius(1)
		, m_sphere(Core::Vec4f(), 1)
		, m_index(UINT32_MAX)
	{ }

	CollisionSphereComponent::~CollisionSphereComponent()
	{ }

	void CollisionSphereComponent::OnStartGame()
	{
		const TransformComponent& transform = GetOwner()->GetTransform();
		m_sphere.SetCenter(transform.GetWorldTx().GetT() + m_center);
		m_sphere.SetRadius(m_radius);

		CollisionSubsystem* pCollision = CollisionSubsystem::GetSubsystem();
		m_index = pCollision->AddShape(&m_sphere);
	}

	void CollisionSphereComponent::OnDestroyGame()
	{
		CollisionSubsystem* pCollision = CollisionSubsystem::GetSubsystem();
		pCollision->RemoveShape(m_index);
	}

	void CollisionSphereComponent::Update(float dt)
	{
		const TransformComponent& transform = GetOwner()->GetTransform();
		m_sphere.SetCenter(transform.GetWorldTx().GetT() + m_center);
	}
}
