/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Game/Subsystems/Collision/Shapes/ShapeType.h"

namespace Systems
{
	class GameObject;

	class ICollisionShape
	{
	public:
		ICollisionShape(ShapeType type);
		~ICollisionShape();

		ShapeType GetType() const;

	private:
		ShapeType m_type;
	};
}
