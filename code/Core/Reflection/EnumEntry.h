/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>
#include <string>

namespace Core
{
	class EnumEntry
	{
	public:
		EnumEntry()
			: m_value(-1)
			, m_name()
		{ }

		EnumEntry(int64_t value, const std::string& name)
			: m_value(value)
			, m_name(name)
		{ }

		~EnumEntry()
		{ }

	private:
		int64_t m_value;
		std::string m_name;
	};
}
