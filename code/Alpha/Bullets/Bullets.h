/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class Vec4f;
}

enum class BulletType : uint8_t
{
	NORMAL,
	COUNTERABLE,

	COUNT
};

enum class BulletState : uint8_t
{
	ATTACK,
	COUNTER,

	COUNT
};

class Bullets
{
public:
	Core::Vec4f* m_positions;
	Core::Vec4f* m_speed;
	Core::Vec4f* m_acceleration;
	float* m_timeToLive;
	bool* m_free;
	BulletType* m_type;
	BulletState* m_state;

	const uint32_t COUNT = 2000;

	void Create();
	void Delete();

	uint32_t Allocate(uint32_t count);
	void Free(uint32_t start, uint32_t count);
};
