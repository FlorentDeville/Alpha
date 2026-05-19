/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Bezier/Bezier.h"

namespace Core
{
	QuadraticBezier::QuadraticBezier(const Core::Vec4f& p0, const Core::Vec4f& p1, const Core::Vec4f& p2)
		: m_p0(p0)
		, m_p1(p1)
		, m_p2(p2)
	{ }

	Core::Vec4f QuadraticBezier::Evaluate(float t) const
	{
		float oneMinusT = 1 - t;

		return (m_p0 * oneMinusT * oneMinusT) + (m_p1 * 2 * oneMinusT * t) + (m_p2 * t * t);
	}

	Core::Vec4f QuadraticBezier::EvaluateFirstDerivative(float t) const
	{
		return ((m_p1 - m_p0) * 2 * (1 - t)) + ((m_p2 - m_p1) * 2 * t);
	}
}
