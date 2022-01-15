/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

enum MessageId
{
	M_MouseMove,	//The mouse has moved
	M_MouseEnter,	//The mouse entered the area of a widget
	M_MouseExit,	//The mouse exited the area of a widget
	M_MouseLDown,	//The mouse left button is down
	M_MouseLUp,		//The mouse left button is up
	M_KeyDown,
	M_Count
};

//Button pressed in a M_MouseMove event
enum MouseKey
{
	M_None	  = 0x00,
	M_Control = 0x01,
	M_LButton = 0x02
};
