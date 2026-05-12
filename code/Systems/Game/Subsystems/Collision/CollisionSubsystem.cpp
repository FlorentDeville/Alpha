/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Collision/CollisionSubsystem.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Collision/Shapes/ICollisionShape.h"
#include "Systems/Game/Subsystems/Collision/Shapes/ShapeSphere.h"

namespace Systems
{
	uint32_t CollisionSubsystem::m_subsystemIndex = UINT32_MAX;

	CollisionSubsystem::CollisionSubsystem()
		: ISubsystem()
		, m_shapes()
	{ }

	CollisionSubsystem::~CollisionSubsystem()
	{ }

	void CollisionSubsystem::Update(const GameContext& context)
	{
		for (uint32_t ii = 0; ii < m_shapes.GetSize(); ++ii)
		{
			const ICollisionShape* pShape1 = m_shapes[ii];

			for (uint32_t jj = ii + 1; jj < m_shapes.GetSize(); ++jj)
			{
				const ICollisionShape* pShape2 = m_shapes[jj];

				if (pShape1->GetType() == ShapeType::SPHERE && pShape2->GetType() == ShapeType::SPHERE)
				{
					bool res = CollisionSphereVsSphere(static_cast<const ShapeSphere*>(pShape1), static_cast<const ShapeSphere*>(pShape2));
					Core::LogModule::Get().LogInfo("Collision detected");

					//callback event OnCollision(const ICollisionShape* other)
				}
			}
		}
	}

	uint32_t CollisionSubsystem::AddShape(ICollisionShape* pShape)
	{
		uint32_t index = FindFreeSlot();
		m_shapes[index] = pShape;
		return index;
	}

	void CollisionSubsystem::RemoveShape(uint32_t index)
	{
		if (!m_shapes.IsValidIndex(index))
			return;

		m_shapes[index] = nullptr;
	}

	CollisionSubsystem* CollisionSubsystem::GetSubsystem()
	{
		ISubsystem* pSubsystem = GameMgr::Get().GetGameSubsystem(m_subsystemIndex);
		return static_cast<CollisionSubsystem*>(pSubsystem);
	}

	uint32_t CollisionSubsystem::FindFreeSlot()
	{
		for (uint32_t ii = 0; ii < m_shapes.GetSize(); ++ii)
		{
			if (m_shapes[ii] == nullptr)
				return ii;
		}

		uint32_t freeSlot = m_shapes.GetSize();
		m_shapes.PushBack(nullptr);
		return freeSlot;
	}

	bool CollisionSubsystem::CollisionSphereVsSphere(const ShapeSphere* pS1, const ShapeSphere* pS2) const
	{
		Core::Vec4f centerVector = pS1->GetCenter() - pS2->GetCenter();
		float centerDistanceSquare = centerVector.Length2();

		float sumRadiusSquare = pS1->GetRadius() + pS2->GetRadius();
		sumRadiusSquare = sumRadiusSquare * sumRadiusSquare;

		if (centerDistanceSquare < sumRadiusSquare)
			return true;

		return false;
	}
}
