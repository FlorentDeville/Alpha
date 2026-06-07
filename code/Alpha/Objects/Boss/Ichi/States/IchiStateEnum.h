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

	PHASE1_TO_PHASE2,

	PHASE2_TRAVEL,
	PHASE2_ATTACK1,
	PHASE2_ATTACK2,

	PHASE2_TO_PHASE3,

	PHASE3_TRAVEL,
	PHASE3_ATTACK1,
	PHASE3_ATTACK2,

	DYING,

	COUNT
};
