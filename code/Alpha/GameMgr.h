/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"

namespace Rendering
{
	class RenderTarget;
}

namespace Systems
{
	class LevelAsset;
	class RenderPassBase;
	class RenderPassShadowMaps;
}

class GameMgr : public Core::Singleton<GameMgr>
{
public:
	GameMgr();
	~GameMgr();

	void Init();
	void Release();

	void Update();
	void Render();

	void RequestLoadingLevel(Systems::NewAssetId levelId);
	void RequestUnloadingLevel(Systems::NewAssetId levelId);

	void RequestUnloadingAllLevels();

private:
	Core::Array<Systems::NewAssetId> m_loadedLevelsIds;		//currently loaded level
	Core::Array<Systems::LevelAsset*> m_loadedLevels;		//pointers to the currently loaded levels.

	Core::Array<Systems::NewAssetId> m_loadingRequest;
	Core::Array<Systems::NewAssetId> m_unloadingRequest;

	Rendering::RenderTarget* m_pFinalRenderTarget;

	Systems::RenderPassBase* m_pRenderPassBase;
	Systems::RenderPassShadowMaps* m_pRenderPassShadowMaps;

	bool IsLevelAlreadyLoaded(Systems::NewAssetId id) const;

	void ExecuteLoadingRequests();
	void ExecuteUnloadingRequests();
};
