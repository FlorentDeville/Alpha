/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

#include "OsWin/VirtualKeyCode.h"

namespace Os
{
	int16_t GetKeyState(VKeyCode vkey);

	bool IsKeyDown(VKeyCode vkey);
	bool IsKeyUp(VKeyCode vkey);
}
