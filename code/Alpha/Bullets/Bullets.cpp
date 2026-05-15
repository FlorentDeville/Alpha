/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/Bullets.h"

#include "Core/Math/Vec4f.h"

void Bullets::Create()
{
	m_positions = new Core::Vec4f[COUNT];
	m_speed = new Core::Vec4f[COUNT];
	m_acceleration = new Core::Vec4f[COUNT];
	m_timeToLive = new float[COUNT];
	m_free = new bool[COUNT] { true };
	m_type = new BulletType[COUNT];
	m_state = new BulletState[COUNT];
}

void Bullets::Delete()
{
	delete[] m_positions;
	delete[] m_speed;
	delete[] m_acceleration;
	delete[] m_free;
	delete[] m_timeToLive;
	delete[] m_type;
	delete[] m_state;
}

uint32_t Bullets::Allocate(uint32_t count)
{
	uint32_t index = UINT32_MAX;
	uint32_t found = 0;
	for (uint32_t ii = 0; ii < COUNT; ++ii)
	{
		if (m_free[ii])
		{
			found = 0;
			continue;
		}
		else
		{
			++found;
		}

		if (found >= count)
		{
			index = ii - found + 1;
			break;
		}
	}

	if (index < 0)
		return index;

	for (uint32_t ii = index; ii < count; ++ii)
		m_free[ii] = false;

	return index;
}

void Bullets::Free(uint32_t start, uint32_t count)
{
	for (uint32_t ii = start; ii < start + count; ++ii)
		m_free[ii] = true;
}
