/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Systems
{
	class ObjectId
	{
	public:
		ObjectId();
		explicit ObjectId(uint64_t id);
		~ObjectId();

		uint64_t ToUint64() const;

		static const ObjectId INVALID;
	private:
		uint64_t m_id;
	};
}
