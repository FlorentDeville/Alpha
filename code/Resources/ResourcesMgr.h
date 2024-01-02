/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Resources/AppResourceId.h"

#include <cstdint>
#include <string>

namespace AppResources
{
	class ResourcesMgr : public Core::Singleton<ResourcesMgr>
	{
	public:
		ResourcesMgr();
		~ResourcesMgr();

		void Init() override;
		void Shutdown() override;

		int16_t GetSystemResourceId(AppResourceId appId) const;
		const char* GetSystemResourceType(AppResourceId appId) const;
	};
}
