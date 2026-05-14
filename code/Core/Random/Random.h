/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>
#include <random>

namespace Core
{
	class RandomUInt
	{
	public:
		RandomUInt(uint32_t min = 0, uint32_t max = UINT32_MAX);
		~RandomUInt() = default;

		uint32_t Generate();

	private:
		std::random_device m_rd;
		std::mt19937 m_gen;
		std::uniform_int_distribution<uint32_t> m_dis;
	};

	class RandomFloat
	{
	public:
		RandomFloat(float min = FLT_MIN, float max = FLT_MAX);
		~RandomFloat() = default;

		float Generate();

	private:
		std::random_device m_rd;
		std::mt19937 m_gen;
		std::uniform_real_distribution<float> m_dis;
	};
}
