/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class LevelAsset;
	class GameContext;

	void InstanciateLevel(LevelAsset* pLevel, GameContext* pWorld);
	void DeleteInstanciatedLevel(LevelAsset* pLevel, GameContext* pWorld);
}
