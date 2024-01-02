/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Resources/ResourcesMgr.h"

#include "Resources/resource.h"

namespace Resources
{
	ResourcesMgr::ResourcesMgr()
	{}

	ResourcesMgr::~ResourcesMgr()
	{}

	int16_t ResourcesMgr::GetApplicationIconResourceId() const
	{
		return REC_APP_ICON;
	}
}
