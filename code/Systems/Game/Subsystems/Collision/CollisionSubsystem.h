/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Game/Subsystems/ISubsystem.h"

namespace Systems
{
	class ICollisionShape;
	class ShapeSphere;

	class CollisionSubsystem : public ISubsystem
	{
	public:
		CollisionSubsystem();
		~CollisionSubsystem();

		void Update(const GameContext& context) override;

		//Add a shape to the collision detection. The shape does not belong to the subsystem and will not be deleted by it.
		uint32_t AddShape(ICollisionShape* pShape);
		void RemoveShape(uint32_t index);

		bool CollisionDetection(const ICollisionShape* pShape, const ICollisionShape** pOutOther) const;

		static uint32_t m_subsystemIndex;
		static CollisionSubsystem* GetSubsystem();

	private:
		Core::Array<ICollisionShape*> m_shapes;

		uint32_t FindFreeSlot();

		bool CollisionSphereVsSphere(const ShapeSphere* pS1, const ShapeSphere* pS2) const;
	};
}
