/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Collision/Shapes/ICollisionShape.h"

namespace Systems
{
	ICollisionShape::ICollisionShape(ShapeType type)
		: m_type(type)
	{ }

	ICollisionShape::~ICollisionShape()
	{ }

	ShapeType ICollisionShape::GetType() const
	{
		return m_type;
	}
}
