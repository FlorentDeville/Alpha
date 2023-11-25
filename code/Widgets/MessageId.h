/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

enum MessageId
{
	M_Invalid = -1,
	M_MouseMove,	//The mouse has moved
	M_MouseEnter,	//The mouse entered the area of a widget
	M_MouseExit,	//The mouse exited the area of a widget
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

//Button pressed in a M_MouseMove event
enum MouseKey
{
	M_None	  = 0x00,
	M_Control = 0x01,
	M_LButton = 0x02,
	M_MButton = 0x03,
	M_RButton = 0x04
};
