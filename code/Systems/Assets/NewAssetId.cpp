/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	const NewAssetId NewAssetId::INVALID = NewAssetId();

	NewAssetId::NewAssetId()
		: m_id(-1)
	{ }

	NewAssetId::NewAssetId(uint64_t id)
		: m_id(id)
	{ }

	NewAssetId::NewAssetId(ContainerId containerId, uint64_t objectId)
	{
		m_id = containerId.ToUint64() | objectId << 56;
	}

	NewAssetId::~NewAssetId()
	{ }

	std::string NewAssetId::ToString() const
	{
		const int MAX_SIZE = 16;
		std::string buffer;
		buffer.resize(MAX_SIZE);
		snprintf(&buffer[0], MAX_SIZE, "%016llx", m_id);
		return buffer;
	}

	bool NewAssetId::operator<(NewAssetId other) const
	{
		return m_id < other.m_id;
	}
}
