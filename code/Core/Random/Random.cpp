/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Random/Random.h"

namespace Core
{
	RandomUInt::RandomUInt(uint32_t min, uint32_t max)
		: m_rd()
		, m_gen()
		, m_dis(min, max)
	{
		m_gen.seed(m_rd());
	}

	uint32_t RandomUInt::Generate()
	{
		return m_dis(m_gen);
	}

	RandomFloat::RandomFloat(float min, float max)
		: m_rd()
		, m_gen()
		, m_dis(min, max)
	{ 
		m_gen.seed(m_rd());
	}
	
	float RandomFloat::Generate()
	{
		return m_dis(m_gen);
	}
}
