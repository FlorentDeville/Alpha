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

		static int64_t ToStoredValue(uint64_t raw);
		static int64_t ToStoredValue(uint32_t raw);
		static int64_t ToStoredValue(int32_t raw);
		static int64_t ToStoredValue(uint16_t raw);
		static int64_t ToStoredValue(int16_t raw);
		static int64_t ToStoredValue(uint8_t raw);
		static int64_t ToStoredValue(int8_t raw);

	private:
		int64_t m_value;
		std::string m_name;
	};
}
