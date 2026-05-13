/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Collision/Shapes/ICollisionShape.h"

namespace Systems
{
	ICollisionShape::ICollisionShape(GameObject* pOwner, ShapeType type)
		: m_type(type)
		, m_pOwner(pOwner)
	{ }

	ICollisionShape::~ICollisionShape()
	{ }

	ShapeType ICollisionShape::GetType() const
	{
		return m_type;
	}

	GameObject* ICollisionShape::GetOwner() const
	{
		return m_pOwner;
	}

	void ICollisionShape::SetOwner(GameObject* pOwner)
	{
		m_pOwner = pOwner;
	}
}
