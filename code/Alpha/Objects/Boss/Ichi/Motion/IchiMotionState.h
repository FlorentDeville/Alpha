/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

class IchiMotionState
{
public:
	enum Type
	{
		IDLE,
		TRAVEL,
		STOP,
		STRAFE,

		COUNT
	};
};
