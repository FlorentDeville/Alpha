/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <vector>

class GameEntity;
class CameraEntity;

namespace DirectX
{
	struct XMMATRIX;
}

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

	const DirectX::XMMATRIX& GetView() const;
	const DirectX::XMMATRIX& GetProjection() const;

private:
	GameMgr();
	~GameMgr();

	std::vector<GameEntity*> m_entities;

	CameraEntity* m_pCurrentCamera; //this could be a stack of cameras but I don't need this for now
};
