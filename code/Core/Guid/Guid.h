/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class Guid
	{
	public:
		Guid();
		Guid(const char* pBuffer);
		~Guid();

		bool operator==(const Guid& other) const;
		bool operator!=(const Guid& other) const;
		
		bool operator<(const Guid& other) const;

		bool IsValid() const;

		//Buffer must be at least 37 characters (36 for the guid + 1 for \0).
		bool ToString(char* pBuffer, uint32_t bufferSize) const;

		static Guid GenerateNewGuid();

	private:
		uint32_t m_data1;
		uint16_t m_data2;
		uint16_t m_data3;
		uint64_t m_data4;
	};
}
