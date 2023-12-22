/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "OsWin/Input.h"

#define LEAN_AND_MEAN
#include <Windows.h>

namespace Os
{
	int16_t GetKeyState(VKeyCode vkey)
	{
		return ::GetKeyState(vkey);
	}

	bool IsKeyDown(VKeyCode vkey)
	{
		int16_t res = GetKeyState(vkey);
		if (res & 0x8000)
			return true;

		return false;
	}

	bool IsKeyUp(VKeyCode vkey)
	{
		return !IsKeyDown(vkey);
	}

	bool GetMousePosition(int32_t& x, int32_t& y)
	{
		POINT point;
		bool res = GetCursorPos(&point);
		x = point.x;
		y = point.y;
		return res;
	}
}