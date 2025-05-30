/********************************************************************/
/* � 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
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
		s_appResMap[kAppIcon] =			{ RES_APP_ICON,			kAppIcon,			"ICON" };
		s_appResMap[kUiIconExpanded] =	{ RES_ICON_EXPANDED,	kUiIconExpanded,	"PNG" };
		s_appResMap[kUiIconCollapsed] = { RES_ICON_COLLAPSED,	kUiIconCollapsed,	"PNG" };
		s_appResMap[kUiIconClose] =		{ RES_ICON_CLOSE,		kUiIconClose,		"PNG" };
		s_appResMap[kUiIconCloseHover] ={ RES_ICON_CLOSE_HOVER,	kUiIconCloseHover,	"PNG" };
		s_appResMap[kUiIconCursor] =	{ RES_ICON_CURSOR,		kUiIconCursor,		"PNG" };
		s_appResMap[kUiIconCheckmark] = { RES_ICON_CHECKMARK,	kUiIconCheckmark,	"PNG" };
		s_appResMap[kUiIconImport] =	{ RES_ICON_IMPORT,		kUiIconImport,		"PNG" };
		
		s_appResMap[kFontDefSegoeUI] =	{ RES_FONT_DEF_SEGOEUI,	kFontDefSegoeUI,	"TEXT" };
		s_appResMap[kFontTexSegoeUI] =	{ RES_FONT_TEX_SEGOEUI,	kFontTexSegoeUI,	"PNG" };
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
