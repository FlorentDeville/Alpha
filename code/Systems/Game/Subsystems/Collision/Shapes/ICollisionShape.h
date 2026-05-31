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
		ICollisionShape(GameObject* pOwner, ShapeType type);
		~ICollisionShape();

		ShapeType GetType() const;
		
		//What out, it can be null.
		GameObject* GetOwner() const;

		void SetOwner(GameObject* pOwner);

		EVENT_DECL(Collision, void(const ICollisionShape* pOther))

	private:
		GameObject* m_pOwner;
		ShapeType m_type;
	};
}
