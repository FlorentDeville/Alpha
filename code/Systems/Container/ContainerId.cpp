/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Container/ContainerId.h"

namespace Systems
{
	const ContainerId ContainerId::INVALID = ContainerId();

	ContainerId::ContainerId()
		: m_id(-1)
	{ }

	ContainerId::ContainerId(uint64_t id)
		: m_id(id)
	{
		m_id = id & 0x00FFFFFFFFFFFFFF;
	}

	ContainerId::ContainerId(const char* strId)
	{
		sscanf_s(strId, "%016llx", &m_id);
	}

	bool ContainerId::operator==(ContainerId other) const
	{
		return m_id == other.m_id;
	}

	bool ContainerId::operator!=(ContainerId other) const
	{
		return !(*this == other);
	}

	bool ContainerId::operator<(ContainerId other) const
	{
		return m_id < other.m_id;
	}

	bool ContainerId::IsValid() const
	{
		return *this != INVALID;
	}

	std::string ContainerId::ToString() const
	{
		const int MAX_SIZE = 17; //16 characters plus \0.
		char buffer[MAX_SIZE] = { '\0' };
		snprintf(buffer, MAX_SIZE, "%016llx", m_id);
		return std::string(buffer);
	}

	uint64_t ContainerId::ToUint64() const
	{
		return m_id;
	}

}
