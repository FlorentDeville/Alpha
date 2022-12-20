/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/Asset.h"

namespace Systems
{
	Asset::Asset(AssetId id, const std::string& virtualName, const std::string& path, AssetType type)
		: m_id(id)
		, m_virtualName(virtualName)
		, m_path(path)
		, m_type(type)
	{}

	Asset::~Asset()
	{}

	AssetId Asset::GetId() const
	{
		return m_id;
	}

	const std::string& Asset::GetVirtualName() const
	{
		return m_virtualName;
	}

	const std::string& Asset::GetPath() const
	{
		return m_path;
	}

	AssetType Asset::GetType() const
	{
		return m_type;
	}
}
