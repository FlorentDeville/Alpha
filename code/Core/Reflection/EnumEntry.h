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
		EnumEntry();

		explicit EnumEntry(int64_t value, const std::string& name);
		explicit EnumEntry(uint64_t value, const std::string& name);
		explicit EnumEntry(int32_t value, const std::string& name);
		explicit EnumEntry(uint32_t value, const std::string& name);
		explicit EnumEntry(int16_t value, const std::string& name);
		explicit EnumEntry(uint16_t value, const std::string& name);
		explicit EnumEntry(int8_t value, const std::string& name);
		explicit EnumEntry(uint8_t value, const std::string& name);

		~EnumEntry();

		const std::string& GetName() const;
		int64_t GetValue() const;

	private:
		int64_t m_value;
		std::string m_name;
	};
}
