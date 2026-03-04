/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>
#include <string>

namespace Core
{
	//An enum entry represent a single enumeration value.
	//Internally the enum is stored as an int64_t. The real enum is stored as a bit copy inside m_value.
	//ToStoredValue converts a real enum to the internally stored value.
	//ToXByte converts from the internally stored value to a integer with the same size as the enum.
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

		static int8_t To1Byte(int64_t storedValue);
		static int16_t To2Bytes(int64_t storedValue);
		static int32_t To4Bytes(int64_t storedValue);

	private:
		int64_t m_value;
		std::string m_name;
	};
}
