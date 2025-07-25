/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace OsWin
{
	//Mask used by windows to indentify keys and mouse buttons.
	enum UIMouseMask
	{
		Unknown = -1,
		LeftButton = 0x01,
		RightButton = 0x02,
		Shift = 0x04,
		Control = 0x08,
		MiddleButton = 0x10,
		XButton1 = 0x20,
		XButton2 = 0x40
	};
}
