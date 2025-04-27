/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetType/NewAssetType.h"

namespace Systems
{
	NewAssetType::NewAssetType()
		: m_name()
		, m_sid(Core::INVALID_SID)
	{ }

	NewAssetType::NewAssetType(const std::string& name)
		: m_name()
	{
		SID(m_name);
	}

	NewAssetType::NewAssetType(const std::string& name, Core::Sid sid)
		: m_name(name)
		, m_sid(sid)
	{ }

	NewAssetType::~NewAssetType()
	{ }

	const std::string& NewAssetType::GetName() const
	{
		return m_name;
	}

	Core::Sid NewAssetType::GetSid() const
	{
		return m_sid;
	}
}
