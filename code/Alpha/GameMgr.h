/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Rendering/Material/MaterialId.h"
#include "Rendering/Mesh/MeshId.h"

#include <vector>

class GameEntity;
class CameraEntity;

namespace DirectX
{
	struct XMMATRIX;
}

namespace Systems
{
	class Scene;
}

class GameMgr : public Core::Singleton<GameMgr>
{
public:
	GameMgr();
	~GameMgr();

	void Init();
	void Release();

	void CreatePlayerEntity(Rendering::MeshId meshId, Rendering::MaterialId materialId);
	void CreateCameraEntity(float aspectRatio);
	void CreateBackgroundEntity(Rendering::MeshId meshId, Rendering::MaterialId materialId);

	void Update();
	void Render();

	const DirectX::XMMATRIX& GetView() const;
	const DirectX::XMMATRIX& GetProjection() const;

	std::vector<GameEntity*> m_entities;

	CameraEntity* m_pCurrentCamera; //this could be a stack of cameras but I don't need this for now

	Systems::Scene* m_pScene;
};
