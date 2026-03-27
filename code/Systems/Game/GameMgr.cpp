/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/GameMgr.h"

#include "Core/Math/Constants.h"

#include "Rendering/Camera.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderPassBase.h"
#include "Systems/Rendering/RenderPass/RenderPassShadowMaps.h"

namespace Systems
{
	GameMgr::GameMgr()
		: Core::Singleton<GameMgr>()
		, m_pRenderPassBase(nullptr)
		, m_pRenderPassShadowMaps(nullptr)
		, m_pFinalRenderTarget(nullptr)
	{
	}

	GameMgr::~GameMgr()
	{
	}

	void GameMgr::Init()
	{
		const int WIDTH = 1920;
		const int HEIGHT = 1080;
		constexpr float RATIO = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

		m_pFinalRenderTarget = new Rendering::RenderTarget(WIDTH, HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

		m_pRenderPassShadowMaps = new Systems::RenderPassShadowMaps();

		m_pRenderPassBase = new Systems::RenderPassBase();
		m_pRenderPassBase->SetRenderTarget(m_pFinalRenderTarget);
		m_pRenderPassBase->SetShadowMapRenderTargets(m_pRenderPassShadowMaps->GetRenderTargets(), m_pRenderPassShadowMaps->GetRenderTargetsCount(), m_pRenderPassShadowMaps->GetSrvHeap());

		Rendering::Camera* pDefaultCamera = new Rendering::Camera();
		pDefaultCamera->SetLookAt(Core::Vec4f(0, 10, -10, 1), Core::Vec4f(0, 0, 0, 1), Core::Vec4f(0, 1, 0, 0));
		pDefaultCamera->SetProjection(45 * Core::PI_OVER_180, RATIO, 0.1f, 1000);
		m_cameraStack.push(pDefaultCamera);
	}

	void GameMgr::Release()
	{
		delete m_pRenderPassBase;
		delete m_pRenderPassShadowMaps;
		delete m_pFinalRenderTarget;

		while (!m_cameraStack.empty())
		{
			Rendering::Camera* pCamera = m_cameraStack.top();
			delete pCamera;
			m_cameraStack.pop();
		}
	}

	void GameMgr::Update()
	{
		ExecuteLoadingRequests();
		ExecuteUnloadingRequests();

		for (Systems::LevelAsset* pLevel : m_loadedLevels)
		{
			//going through the list of game object of levels will cause problems when game objects
			//are created at runtime.
			//I should make an internal array of all game object pointers in the GameMgr.
			Core::Array<Systems::GameObject*>& gameObjects = pLevel->GetGameObjectsArray();
			for (Systems::GameObject* pGo : gameObjects)
			{
				pGo->Update();
			}
		}
	}

	void GameMgr::Render()
	{
		Systems::RenderableScene scene;

		const Rendering::Camera* pCamera = m_cameraStack.top();
		Systems::PrepareRenderableCamera(pCamera->GetViewMatrix(), pCamera->GetProjectionMatrix(), pCamera->GetPosition(), pCamera->GetFOV(), scene);

		for (Systems::LevelAsset* pLevel : m_loadedLevels)
		{
			//going through the list of game object of levels will cause problems when game objects
			//are created at runtime.
			//I should make an internal array of all game object pointers in the GameMgr.
			Core::Array<Systems::GameObject*>& roots = pLevel->GetRootGameObjects();
			for (Systems::GameObject* pGo : roots)
				pGo->UpdateTransform();

			Systems::PrepareRenderableScene(pLevel, scene);
		}

		//call the render pass and render the scene.
		m_pRenderPassShadowMaps->PreRender(scene);
		m_pRenderPassShadowMaps->Render(scene);
		m_pRenderPassShadowMaps->PostRender(scene);

		//now render the level editor view of the scene.
		m_pRenderPassBase->PreRender(scene);
		m_pRenderPassBase->Render(scene);
		m_pRenderPassBase->PostRender(scene);
	}

	void GameMgr::RequestLoadingLevel(Systems::NewAssetId levelId)
	{
		m_loadingRequest.PushBack(levelId);
	}

	void GameMgr::RequestUnloadingLevel(Systems::NewAssetId levelId)
	{
		m_unloadingRequest.PushBack(levelId);
	}

	void GameMgr::RequestUnloadingAllLevels()
	{
		m_unloadingRequest = m_loadedLevelsIds;
	}

	Rendering::RenderTarget* GameMgr::GetFinalRenderTarget()
	{
		return m_pFinalRenderTarget;
	}

	bool GameMgr::IsLevelAlreadyLoaded(Systems::NewAssetId id) const
	{
		for (const Systems::NewAssetId loadedId : m_loadedLevelsIds)
		{
			if (id == loadedId)
				return true;
		}

		return false;
	}

	void GameMgr::ExecuteLoadingRequests()
	{
		for (const Systems::NewAssetId id : m_loadingRequest)
		{
			if (IsLevelAlreadyLoaded(id))
				continue;

			Systems::LevelAsset* pLevel = Systems::AssetUtil::LoadAsset<Systems::LevelAsset>(id);
			if (!pLevel)
				return; //doesn't exist

			m_loadedLevelsIds.PushBack(id);
			m_loadedLevels.PushBack(pLevel);
		}

		m_loadingRequest.Clear();
	}

	void GameMgr::ExecuteUnloadingRequests()
	{
		for (const Systems::NewAssetId id : m_unloadingRequest)
		{
			if (!IsLevelAlreadyLoaded(id))
				continue;

			Systems::LevelAsset* pLevel = Systems::AssetUtil::GetAsset<Systems::LevelAsset>(id);
			if (!pLevel)
				return; //doesn't exist

			m_loadedLevelsIds.Erase(id);
			m_loadedLevels.Erase(pLevel);
		}

		m_unloadingRequest.Clear();
	}
}
