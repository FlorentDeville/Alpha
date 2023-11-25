/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Widgets
{
	enum EventId
	{
		kUnknown = -1,

		//Mouse move event
		kMouseMove,
		kMouseEnter,	//The mouse entered the area of a widget
		kMouseExit,		//The mouse exited the area of a widget

		//Mouse button up/down
		kMouseLDown,	//The mouse left button is down
		kMouseLUp,		//The mouse left button is up
		kMouseMDown,
		kMouseMUp,
		kMouseRDown,
		kMouseRUp,

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
