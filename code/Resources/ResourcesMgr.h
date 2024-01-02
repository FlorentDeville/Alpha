/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <cstdint>

namespace Resources
{
	class ResourcesMgr : public Core::Singleton<ResourcesMgr>
	{
	public:
		ResourcesMgr();
		~ResourcesMgr();

		int16_t GetApplicationIconResourceId() const;
	};
}
