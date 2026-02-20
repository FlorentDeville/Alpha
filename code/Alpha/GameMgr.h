/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

class GameMgr : public Core::Singleton<GameMgr>
{
public:
	GameMgr();
	~GameMgr();

	void Init();
	void Release();

	void Update();
	void Render();
};
