/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Container/ObjectId.h"

namespace Systems
{
	const ObjectId ObjectId::INVALID = ObjectId();

	ObjectId::ObjectId()
		: m_id(-1)
	{ }

	ObjectId::ObjectId(uint64_t id)
		: m_id(id)
	{ }

	ObjectId::~ObjectId()
	{ }

	uint64_t ObjectId::ToUint64() const
	{
		return m_id;
	}
}