/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/Metadata/AssetMetadata.h"

namespace Systems
{
	AssetMetadata::AssetMetadata()
		: m_id()
		, m_virtualName()
		, m_classSid()
	{ }

	AssetMetadata::AssetMetadata(NewAssetId id, const std::string& virtualName, Core::Sid classSid)
		: m_id(id)
		, m_virtualName(virtualName)
		, m_classSid(classSid)
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

	Core::Sid AssetMetadata::GetClassSid() const
	{
		return m_classSid;
	}
}
