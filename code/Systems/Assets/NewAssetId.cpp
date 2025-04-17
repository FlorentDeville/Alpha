/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/NewAssetId.h"

#include <iomanip>
#include <sstream>

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
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::setw(16) << m_id;
		return ss.str();
	}

	bool NewAssetId::operator<(NewAssetId other) const
	{
		return m_id < other.m_id;
	}
}
