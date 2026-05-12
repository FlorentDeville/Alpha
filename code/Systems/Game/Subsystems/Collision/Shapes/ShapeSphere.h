/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

#include "Systems/Game/Subsystems/Collision/Shapes/ICollisionShape.h"

namespace Systems
{
	class ShapeSphere : public ICollisionShape
	{
	public:
		ShapeSphere(const Core::Vec4f& center, float radius);
		~ShapeSphere();

		void SetCenter(const Core::Vec4f& center);
		void SetRadius(float radius);

		const Core::Vec4f& GetCenter() const;
		float GetRadius() const;

	private:
		Core::Vec4f m_center;
		float m_radius;
	};
}
