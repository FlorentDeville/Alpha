/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Resources/ResourcesMgr.h"

#include "Resources/resource.h"

#include <map>

namespace AppResources
{
	struct AppResource
	{
		int16_t m_sysId;
		AppResourceId m_appId;
		std::string m_type;
	};

	std::map<AppResourceId, AppResource> s_appResMap;

	ResourcesMgr::ResourcesMgr()
	{}

	ResourcesMgr::~ResourcesMgr()
	{}

	void ResourcesMgr::Init()
	{
		s_appResMap[kAppIcon] =			{ REC_APP_ICON,			kAppIcon,			"ICON" };
		s_appResMap[kUiIconExpanded] =	{ REC_ICON_EXPANDED,	kUiIconExpanded,	"PNG" };
	}

	void ResourcesMgr::Shutdown()
	{
		s_appResMap.clear();
	}

	int16_t ResourcesMgr::GetSystemResourceId(AppResourceId appId) const
	{
		return s_appResMap[appId].m_sysId;
	}

	const char* ResourcesMgr::GetSystemResourceType(AppResourceId appId) const
	{
		return s_appResMap[appId].m_type.c_str();
	}
}
