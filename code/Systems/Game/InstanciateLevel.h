/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class LevelAsset;
	class World;

	void InstanciateLevel(LevelAsset* pLevel, World* pWorld);
	void DeleteInstanciatedLevel(LevelAsset* pLevel, World* pWorld);
}
