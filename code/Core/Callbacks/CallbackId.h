/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class CallbackId
	{
	public:
		CallbackId();
		CallbackId(uint64_t);
		~CallbackId();

		bool IsValid() const;

		bool operator==(const CallbackId other) const;
		bool operator<(const CallbackId other) const;

		uint64_t m_id;
	};
}
