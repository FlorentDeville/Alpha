/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

enum MessageId
{
	M_Invalid = -1,
	M_MouseMove,	//The mouse has moved
	M_MouseLDown,	//The mouse left button is down
	M_MouseLUp,		//The mouse left button is up
	M_MouseMDown,
	M_MouseMUp,
	M_MouseRDown,
	M_MouseRUp,
	M_VirtualKeyDown,	//Virtual Key : the message is the virtual key code
	M_VirtualKeyUp,		//Virtual Key : the message is the virtual key code
	M_CharKeyDown,		//Character key : the message is the character
	M_Count
};
