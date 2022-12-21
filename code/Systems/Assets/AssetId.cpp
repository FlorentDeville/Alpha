/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetId.h"

#include <cstdint>

namespace Systems
{
	AssetId::AssetId()
	{
		m_id = UINT64_MAX; //invalid by default
	}

	AssetId::AssetId(size_t id)
		: m_id(id)
	{}

	bool AssetId::operator==(const AssetId id) const
	{
		return id.m_id == m_id;
	}

	bool AssetId::operator!=(const AssetId id) const
	{
		return id.m_id != m_id;
	}

	bool AssetId::operator<(const AssetId id) const
	{
		return m_id < id.m_id;
	}

	std::string AssetId::ToString() const
	{
		return std::to_string(m_id);
	}

	const AssetId AssetId::INVALID = AssetId(); //the default is -1
}
