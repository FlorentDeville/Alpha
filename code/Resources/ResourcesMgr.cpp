/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Resources/ResourcesMgr.h"

#include "Resources/resource.h"

namespace Resources
{
	ResourcesMgr::ResourcesMgr()
		: m_typePNG("PNG")
	{}

	ResourcesMgr::~ResourcesMgr()
	{}

	int16_t ResourcesMgr::GetApplicationIconResourceId() const
	{
		return REC_APP_ICON;
	}

	int16_t ResourcesMgr::GetIconExpandedResourceId() const
	{
		return REC_ICON_EXPANDED;
	}

	const char* ResourcesMgr::GetIconExpandedResourceType() const
	{
		return m_typePNG.c_str();
	}
}
