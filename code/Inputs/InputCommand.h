/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Inputs
{
	//This should be moved to alpha. Make a GameInput class and register it to the InputMgr at start up.
	enum class InputCommandEnum
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};
}
