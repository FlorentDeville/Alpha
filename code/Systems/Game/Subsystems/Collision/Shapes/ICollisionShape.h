/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include "Systems/Game/Subsystems/Collision/Shapes/ShapeType.h"

namespace Systems
{
	class CollisionSubsystem;
	class GameObject;

	class ICollisionShape
	{
		friend class CollisionSubsystem;

	public:
		ICollisionShape(ShapeType type);
		~ICollisionShape();

		ShapeType GetType() const;

		EVENT_DECL(Collision, void(const ICollisionShape* pOther))

	private:
		ShapeType m_type;
	};
}
