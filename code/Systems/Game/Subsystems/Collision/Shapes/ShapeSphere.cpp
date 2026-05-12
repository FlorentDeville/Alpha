/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Collision/Shapes/ShapeSphere.h"

namespace Systems
{
	ShapeSphere::ShapeSphere(const Core::Vec4f& center, float radius)
		: ICollisionShape(ShapeType::SPHERE)
		, m_center(center)
		, m_radius(radius)
	{ }

	ShapeSphere::~ShapeSphere()
	{ }

	void ShapeSphere::SetCenter(const Core::Vec4f& center)
	{
		m_center = center;
	}

	void ShapeSphere::SetRadius(float radius)
	{
		m_radius = radius;
	}

	const Core::Vec4f& ShapeSphere::GetCenter() const
	{
		return m_center;
	}

	float ShapeSphere::GetRadius() const
	{
		return m_radius;
	}
}
