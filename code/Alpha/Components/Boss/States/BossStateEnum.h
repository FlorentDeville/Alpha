/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

enum BossStateEnum
{
	START,

	PHASE1_TRAVEL,
	PHASE1_ATTACK,

	PHASE1_TO_PHASE2,

	PHASE2_TRAVEL,
	PHASE2_ATTACK1,
	PHASE2_ATTACK2,

	PHASE3_TRAVEL,
	PHASE3_ATTACK1,
	PHASE3_ATTACK2,

	COUNT
};
