/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Os
{
	class Guid
	{
	public:
		Guid();
		~Guid();

		bool operator==(const Guid& other) const;
		bool operator!=(const Guid& other) const;
		
		bool operator<(const Guid& other) const;

		bool IsValid() const;

		static Guid GenerateNewGuid();

	private:
		uint32_t m_data1;
		uint16_t m_data2;
		uint16_t m_data3;
		uint64_t m_data4;
	};
}
