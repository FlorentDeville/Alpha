/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

enum IchiStateEnum
{
	START,

	PHASE1,
	PHASE2,
	PHASE3,

	DYING,
	DEAD,

	COUNT
};
