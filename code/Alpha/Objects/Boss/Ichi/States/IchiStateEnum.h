/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

enum IchiStateEnum
{
	START,

	PHASE1_TRAVEL,
	PHASE1_ATTACK1,
	PHASE1_ATTACK2,

	PHASE2,
	PHASE3,

	DYING,
	DEAD,

	COUNT
};
