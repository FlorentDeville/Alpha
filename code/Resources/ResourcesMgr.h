/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <cstdint>
#include <string>

namespace Resources
{
	class ResourcesMgr : public Core::Singleton<ResourcesMgr>
	{
	public:
		ResourcesMgr();
		~ResourcesMgr();

		int16_t GetApplicationIconResourceId() const;

		int16_t GetIconExpandedResourceId() const;
		const char* GetIconExpandedResourceType() const;

	private:
		std::string m_typePNG;
	};
}
