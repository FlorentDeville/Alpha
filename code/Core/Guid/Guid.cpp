/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Guid/Guid.h"

#include <cstdio>
#include <random>

namespace Core
{
	Guid::Guid()
		: m_data1(0)
		, m_data2(0)
		, m_data3(0)
		, m_data4(0)
	{}

	Guid::Guid(const char* pBuffer)
	{
		uint16_t data41;
		uint16_t data42;
		uint32_t data43;
		sscanf_s(pBuffer, "%x-%hx-%hx-%hx-%4hx%x", &m_data1, &m_data2, &m_data3, &data41, &data42, &data43);

		m_data4 = ((uint64_t)data41 << 48) | ((uint64_t)data42 << 32) | (uint64_t)data43;
	}

	Guid::~Guid()
	{}

	bool Guid::operator==(const Guid& other) const
	{
		if (m_data1 != other.m_data1 || m_data2 != other.m_data2 || m_data3 != other.m_data3 || m_data4 != other.m_data4)
			return false;

		return true;
	}

	bool Guid::operator!=(const Guid& other) const
	{
		return !(*this == other);
	}

	bool Guid::operator<(const Guid& other) const
	{
		if (m_data1 < other.m_data1)
			return true;
		else if (m_data1 > other.m_data2)
			return false;

		if (m_data2 < other.m_data2)
			return true;
		else if (m_data2 > other.m_data2)
			return false;

		if (m_data3 < other.m_data3)
			return true;
		else if (m_data3 > other.m_data3)
			return false;

		if (m_data4 < other.m_data4)
			return true;
		else if (m_data4 > other.m_data4)
			return false;

		return false;
	}

	bool Guid::IsValid() const
	{
		if (m_data1 != 0 || m_data2 != 0 || m_data3 != 0 || m_data4 != 0)
			return true;

		return false;
	}

	bool Guid::ToString(char* pBuffer, uint32_t bufferSize) const
	{
		const int GUID_STRING_LENGTH = 37;
		if (bufferSize < GUID_STRING_LENGTH)
			return false;

		uint16_t data41 = (uint16_t)(m_data4 >> 48);
		uint16_t data42 = (uint16_t)((m_data4 >> 32) & 0x0000FFFFF);
		uint32_t data43 = (uint32_t)(m_data4 & 0x00000000FFFFFFFF);
		snprintf(pBuffer, GUID_STRING_LENGTH, "%x-%hx-%hx-%hx-%hx%x", m_data1, m_data2, m_data3, data41, data42, data43);
		return true;
	}

	Guid Guid::GenerateNewGuid()
	{
		Guid guid;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint16_t> dis(0, UINT16_MAX);
		
		uint32_t data11 = static_cast<uint32_t>(dis(gen));
		uint32_t data12 = static_cast<uint32_t>(dis(gen));
		guid.m_data1 = data11 | (data12 << 16);
		
		guid.m_data2 = static_cast<uint16_t>(dis(gen));
		guid.m_data3 = static_cast<uint16_t>(dis(gen));

		uint64_t data41 = static_cast<uint64_t>(dis(gen));
		uint64_t data42 = static_cast<uint64_t>(dis(gen));
		uint64_t data43 = static_cast<uint64_t>(dis(gen));
		uint64_t data44 = static_cast<uint64_t>(dis(gen));
		guid.m_data4 = data41 | (data42 << 16) | (data43 << 32) | (data43 << 48);

		return guid;
	}
}
