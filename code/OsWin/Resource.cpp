/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "OsWin/Resource.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Os
{
	bool Resource::GetResource(int16_t resourceId, const char* resourceType, char** ptr, uint32_t& size)
	{
		HMODULE hModule = GetModuleHandle(nullptr);
		HRSRC hRsc = FindResource(hModule, MAKEINTRESOURCE(resourceId), resourceType);
		if (hRsc == nullptr)
			return false;

		HGLOBAL hGlobal = LoadResource(hModule, hRsc);
		if (hGlobal == nullptr)
			return false;

		void* p = LockResource(hGlobal);
		if (p == nullptr)
			return false;

		*ptr = static_cast<char*>(p);

		size = SizeofResource(hModule, hRsc);
		if (size == 0)
			return false;

		return true;
	}
}
