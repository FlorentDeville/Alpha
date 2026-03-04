/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Reflection/EnumEntry.h"

namespace Core
{
	EnumEntry::EnumEntry()
		: m_value(-1)
		, m_name()
	{ }

	EnumEntry::EnumEntry(int64_t value, const std::string& name)
		: m_value(value)
		, m_name(name)
	{ }

	EnumEntry::EnumEntry(uint64_t value, const std::string& name)
		: m_name(name)
	{
		m_value = ToStoredValue(value);
	}

	EnumEntry::EnumEntry(int32_t value, const std::string& name)
		: m_name(name)
	{
		m_value = ToStoredValue(value);
	}

	EnumEntry::EnumEntry(uint32_t value, const std::string& name)
		: m_name(name)
	{
		m_value = ToStoredValue(value);
	}

	EnumEntry::EnumEntry(int16_t value, const std::string& name)
		: m_name(name)
	{
		m_value = ToStoredValue(value);
	}

	EnumEntry::EnumEntry(uint16_t value, const std::string& name)
		: m_name(name)
	{
		m_value = ToStoredValue(value);
	}

	EnumEntry::EnumEntry(int8_t value, const std::string& name)
		: m_name(name)
	{
		m_value = ToStoredValue(value);
	}

	EnumEntry::EnumEntry(uint8_t value, const std::string& name)
		: m_name(name)
	{
		m_value = ToStoredValue(value);
	}

	EnumEntry::~EnumEntry()
	{ }

	const std::string& EnumEntry::GetName() const
	{
		return m_name;
	}

	int64_t EnumEntry::GetValue() const
	{
		return m_value;
	}

	int64_t EnumEntry::ToStoredValue(uint64_t raw)
	{
		union Conv
		{
			int64_t o;
			uint64_t i;
		};

		Conv c;
		c.o = 0;
		c.i = raw;
		return c.o;
	}

	int64_t EnumEntry::ToStoredValue(uint32_t raw)
	{
		union Conv
		{
			int64_t o;
			uint32_t i;
		};

		Conv c;
		c.o = 0;
		c.i = raw;
		return c.o;
	}

	int64_t EnumEntry::ToStoredValue(int32_t raw)
	{
		union Conv
		{
			int64_t o;
			int32_t i;
		};

		Conv c;
		c.o = 0;
		c.i = raw;
		return c.o;
	}

	int64_t EnumEntry::ToStoredValue(uint16_t raw)
	{
		union Conv
		{
			int64_t o;
			uint16_t i;
		};

		Conv c;
		c.o = 0;
		c.i = raw;
		return c.o;
	}

	int64_t EnumEntry::ToStoredValue(int16_t raw)
	{
		union Conv
		{
			int64_t o;
			int16_t i;
		};

		Conv c;
		c.o = 0;
		c.i = raw;
		return c.o;
	}

	int64_t EnumEntry::ToStoredValue(uint8_t raw)
	{
		union Conv
		{
			int64_t o;
			uint8_t i;
		};

		Conv c;
		c.o = 0;
		c.i = raw;
		return c.o;
	}

	int64_t EnumEntry::ToStoredValue(int8_t raw)
	{
		union Conv
		{
			int64_t o;
			int8_t i;
		};

		Conv c;
		c.o = 0;
		c.i = raw;
		return c.o;
	}

}
