/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Widgets
{
	enum EventType
	{
		kUnknown = -1,

		//Mouse move event
		kMouseMove,
		kMouseEnter,	//The mouse entered the area of a widget
		kMouseExit,		//The mouse exited the area of a widget

		//Mouse button up/down
		kMouseDown,
		kMouseUp,

		//Mouse click
		kMouseLeftClick,

		//key
		kVKeyDown,
		kVKeyUp,

		kCharKeyDown,

		kFocusChanged,

		kCount

	};
}
