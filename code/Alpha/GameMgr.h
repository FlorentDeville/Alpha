/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

class GameMgr : public Core::Singleton<GameMgr>
{
public:
	void Init();
	void Release();

	void CreatePlayerEntity();
	void CreateCameraEntity();
	void CreateBackgroundEntity();

	void Update();
	void Render();

private:
	GameMgr();
	~GameMgr();
};
