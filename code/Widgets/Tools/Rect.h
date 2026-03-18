/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Widgets
{
	class Rect
	{
	public:
		Rect();
		Rect(int32_t left, int32_t right, int32_t top, int32_t bottom);
		~Rect() = default;

		void Intersect(const Rect& other);

		bool IsInside(int32_t x, int32_t y) const;

		int32_t m_left;
		int32_t m_right;
		int32_t m_top;
		int32_t m_bottom;
	};
}
