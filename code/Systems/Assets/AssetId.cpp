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
		const int MAX_SIZE = 9; //8 characters plus \0.
		char buffer[MAX_SIZE] = { '\0' };
		snprintf(buffer, MAX_SIZE, "%08zu", m_id);
		return std::string(buffer);
	}

	uint64_t AssetId::ToUint64() const
	{
		return m_id;
	}

	const AssetId AssetId::INVALID = AssetId(); //the default is -1
}
