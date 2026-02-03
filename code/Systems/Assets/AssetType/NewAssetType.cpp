/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetType/NewAssetType.h"

namespace Systems
{
	NewAssetType::NewAssetType()
		: m_name()
		, m_sid(Core::INVALID_SID)
		, m_classNameSid(Core::INVALID_SID)
	{ }

	NewAssetType::NewAssetType(const std::string& name, Core::Sid sid, Core::Sid classNameSid)
		: m_name(name)
		, m_sid(sid)
		, m_classNameSid(classNameSid)
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

	Core::Sid NewAssetType::GetClassName() const
	{
		return m_classNameSid;
	}
}
