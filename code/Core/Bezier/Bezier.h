/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

namespace Core
{
	class QuadraticBezier
	{
	public:
		QuadraticBezier(const Core::Vec4f& p0, const Core::Vec4f& p1, const Core::Vec4f& p2);
		~QuadraticBezier() = default;

		Core::Vec4f Evaluate(float t) const;

		Core::Vec4f m_p0;
		Core::Vec4f m_p1;
		Core::Vec4f m_p2;
	};
}
