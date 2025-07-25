/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace OsWin
{
	enum UIMessageId
	{
		Invalid = -1,
		MouseMove,	//The mouse has moved
		MouseLDown,	//The mouse left button is down
		MouseLUp,		//The mouse left button is up
		MouseLDoubleClick,
		MouseMDown,
		MouseMUp,
		MouseRDown,
		MouseRUp,
		VirtualKeyDown,	//Virtual Key : the message is the virtual key code
		VirtualKeyUp,		//Virtual Key : the message is the virtual key code
		CharKeyDown,		//Character key : the message is the character. Only handles printable characters
		
		Count
	};
}
