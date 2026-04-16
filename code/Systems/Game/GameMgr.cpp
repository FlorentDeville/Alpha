/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/GameMgr.h"

#include "Core/Math/Constants.h"

#include "Rendering/Camera.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"
#include "Systems/Game/InstanciateLevel.h"
#include "Systems/Game/Subsystems/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Clock/GameClockSubsystem.h"
#include "Systems/Game/World.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Particle/ParticleSystem.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderPassBase.h"
#include "Systems/Rendering/RenderPass/RenderPassBloom.h"
#include "Systems/Rendering/RenderPass/RenderPassShadowMaps.h"

namespace Systems
{
	GameMgr::GameMgr()
		: Core::Singleton<GameMgr>()
		, m_pRenderPassBase(nullptr)
		, m_pRenderPassShadowMaps(nullptr)
		, m_pRenderPassBloom(nullptr)
		, m_pDefaultCamera(nullptr)
		, m_pWorld(nullptr)
	{ }

	GameMgr::~GameMgr()
	{
	}

	void GameMgr::Init()
	{
		const int WIDTH = 1920;
		const int HEIGHT = 1080;
		constexpr float RATIO = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

		m_pRenderPassShadowMaps = new Systems::RenderPassShadowMaps();

		m_pRenderPassBase = new Systems::RenderPassBase(WIDTH, HEIGHT);
		m_pRenderPassBase->SetShadowMapRenderTargets(m_pRenderPassShadowMaps->GetRenderTargets(), m_pRenderPassShadowMaps->GetRenderTargetsCount(), m_pRenderPassShadowMaps->GetSrvHeap());

		m_pRenderPassBloom = new Systems::RenderPassBloom(WIDTH, HEIGHT, 4);
		m_pRenderPassBloom->SetInput(m_pRenderPassBase->GetRenderTarget()->GetColorTexture());
		m_pRenderPassBloom->SetOutput(m_pRenderPassBase->GetRenderTarget());

		m_pDefaultCamera = new Rendering::Camera();
		m_pDefaultCamera->SetLookAt(Core::Vec4f(0, 10, -10, 1), Core::Vec4f(0, 0, 0, 1), Core::Vec4f(0, 1, 0, 0));
		m_pDefaultCamera->SetProjection(45 * Core::PI_OVER_180, RATIO, 0.1f, 1000);

		m_pWorld = new World();
		m_pWorld->m_pCameraSubsystem = new CameraSubsystem();
		m_pWorld->m_pCameraSubsystem->PushCamera(m_pDefaultCamera);

		m_pWorld->m_pParticleSystem = new ParticleSystem();
		m_pWorld->m_pClock = new GameClockSubsystem();
	}

	void GameMgr::Release()
	{
		delete m_pRenderPassBase;
		delete m_pRenderPassShadowMaps;
		delete m_pRenderPassBloom;
		delete m_pDefaultCamera;
		delete m_pWorld;
	}

	void GameMgr::Update(float dt)
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
				pGo->Update(dt);
			}
		}
	}

	void GameMgr::Render()
	{
		Systems::RenderableScene scene;

		const Rendering::Camera* pCamera = m_pWorld->m_pCameraSubsystem->GetTopCamera();
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

		//post process
		m_pRenderPassBloom->PreRender(scene);
		m_pRenderPassBloom->Render(scene);
		m_pRenderPassBloom->PostRender(scene);
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
		m_unloadingRequest.Reserve(m_loadedLevels.GetSize());
		for (const LevelAsset* pLevel : m_loadedLevels)
			m_unloadingRequest.PushBack(pLevel->GetId());
	}

	Rendering::RenderTarget* GameMgr::GetFinalRenderTarget()
	{
		return m_pRenderPassBase->GetRenderTarget();
	}

	bool GameMgr::IsLevelAlreadyLoaded(Systems::NewAssetId id) const
	{
		for (const LevelAsset* pLevel : m_loadedLevels)
		{
			if (id == pLevel->GetId())
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

			Systems::LevelAsset* pLevel = Systems::AssetUtil::GetAsset<Systems::LevelAsset>(id, Systems::LoadingDomain::GAME);
			if (!pLevel)
			{
				pLevel = Systems::AssetUtil::LoadAsset<Systems::LevelAsset>(id, Systems::LoadingDomain::GAME);
				if (!pLevel)
				{
					assert(false && "Tried to load a level that doesn't exist");
					return; //doesn't exist
				}
			}

			m_loadedLevels.PushBack(pLevel);

			InstanciateLevel(pLevel, m_pWorld);
		}

		m_loadingRequest.Clear();
	}

	void GameMgr::ExecuteUnloadingRequests()
	{
		for (const Systems::NewAssetId id : m_unloadingRequest)
		{
			if (!IsLevelAlreadyLoaded(id))
				continue;

			Systems::LevelAsset* pLevel = Systems::AssetUtil::GetAsset<Systems::LevelAsset>(id, Systems::LoadingDomain::GAME);
			if (!pLevel)
			{
				assert(false && "Tried to unload a level that doesn't exist");
				return; //doesn't exist
			}

			DeleteInstanciatedLevel(pLevel, m_pWorld);

			Systems::AssetUtil::UnloadAsset(id, Systems::LoadingDomain::GAME);

			m_loadedLevels.Erase(pLevel);
		}

		m_unloadingRequest.Clear();
	}
}
