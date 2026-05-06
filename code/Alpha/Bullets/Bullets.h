/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

class Bullets
{
public:
	Core::Vec4f* m_positions;
	Core::Vec4f* m_speed;
	Core::Vec4f* m_acceleration;
	float* m_timeToLive;
	bool* m_alive;
};
