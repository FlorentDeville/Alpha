/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"

#include <stack>

namespace Rendering
{
	class Camera;
	class RenderTarget;
}

namespace Systems
{
	class CameraSubsystem;
	class GameObject;
	class ISubsystem;
	class LevelAsset;
	class RenderPassBase;
	class RenderPassBloom;
	class RenderPassShadowMaps;
	class GameContext;

	class GameMgr : public Core::Singleton<GameMgr>
	{
	public:
		GameMgr();
		~GameMgr();

		void Init();
		void Release();

		void Update(float dt);
		void Render();

		void RequestLoadingLevel(Systems::NewAssetId levelId);
		void RequestUnloadingLevel(Systems::NewAssetId levelId);

		void RequestUnloadingAllLevels();

		uint32_t RegisterGameSubsystem(ISubsystem* pSubsystem);

		Rendering::RenderTarget* GetFinalRenderTarget();

		GameContext* GetWorld();

	private:
		//Loading
		Core::Array<Systems::LevelAsset*> m_loadedLevels;		//pointers to the currently loaded levels.

		Core::Array<Systems::NewAssetId> m_loadingRequest;
		Core::Array<Systems::NewAssetId> m_unloadingRequest;

		//Rendering
		Systems::RenderPassBase* m_pRenderPassBase;
		Systems::RenderPassShadowMaps* m_pRenderPassShadowMaps;
		Systems::RenderPassBloom* m_pRenderPassBloom;

		//Camera
		Rendering::Camera* m_pDefaultCamera;

		GameContext* m_pGameContext;

		Core::Array<ISubsystem*> m_gameSubsystems;

		bool IsLevelAlreadyLoaded(Systems::NewAssetId id) const;

		void ExecuteLoadingRequests();
		void ExecuteUnloadingRequests();
	};
}
