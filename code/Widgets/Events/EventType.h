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

		//double click is the second mouse down. The sequence of events is Down, Up, DoubleClick, Up.
		kMouseDoubleClick,

		//key
		kVKeyDown,
		kVKeyUp,

		kCharKeyDown,

		kFocusChanged,

		kCount

	};
}
