/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/Metadata/AssetMetadata.h"

namespace Systems
{
	AssetMetadata::AssetMetadata()
		: m_id()
		, m_virtualName()
		, m_className()
	{ }

	AssetMetadata::AssetMetadata(NewAssetId id, const std::string& virtualName, Core::Sid className)
		: m_id(id)
		, m_virtualName(virtualName)
		, m_className(className)
	{ }

	AssetMetadata::~AssetMetadata()
	{ }

	NewAssetId AssetMetadata::GetAssetId() const
	{
		return m_id;
	}

	const std::string AssetMetadata::GetVirtualName() const
	{
		return m_virtualName;
	}

	Core::Sid AssetMetadata::GetClassName() const
	{
		return m_className;
	}
}
