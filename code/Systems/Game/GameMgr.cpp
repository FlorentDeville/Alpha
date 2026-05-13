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
#include "Systems/Game/Subsystems/Camera/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Clock/GameClockSubsystem.h"
#include "Systems/Game/Subsystems/Collision/CollisionSubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessageSubsystem.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Game/Subsystems/Particle/ParticleSystem.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderPassBase.h"
#include "Systems/Rendering/RenderPass/RenderPassBloom.h"
#include "Systems/Rendering/RenderPass/RenderPassShadowMaps.h"
#include "Systems/Rendering/RenderPass/RenderPassUI.h"

namespace Systems
{
	GameMgr::GameMgr()
		: Core::Singleton<GameMgr>()
		, m_pRenderPassBase(nullptr)
		, m_pRenderPassShadowMaps(nullptr)
		, m_pRenderPassBloom(nullptr)
		, m_pRenderPassUi(nullptr)
		, m_pDefaultCamera(nullptr)
		, m_pGameContext(nullptr)
		, m_gameSubsystems()
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

		m_pRenderPassUi = new Systems::RenderPassUI(WIDTH, HEIGHT);
		m_pRenderPassUi->SetOutput(m_pRenderPassBase->GetRenderTarget());

		m_pDefaultCamera = new Rendering::Camera();
		m_pDefaultCamera->SetLookAt(Core::Vec4f(0, 10, -10, 1), Core::Vec4f(0, 0, 0, 1), Core::Vec4f(0, 1, 0, 0));
		m_pDefaultCamera->SetProjection(45 * Core::PI_OVER_180, RATIO, 0.1f, 1000);

		m_pGameContext = new GameContext();
		m_pGameContext->m_pCameraSubsystem = new CameraSubsystem();
		m_pGameContext->m_pCameraSubsystem->PushCamera(m_pDefaultCamera);

		m_pGameContext->m_pParticleSystem = new ParticleSystem();
		m_pGameContext->m_pClock = new GameClockSubsystem();

		CollisionSubsystem* pCollision = new CollisionSubsystem();
		pCollision->m_subsystemIndex = RegisterGameSubsystem(pCollision);

		GameMessageSubsystem* pMessages = new GameMessageSubsystem();
		pMessages->m_subsystemIndex = RegisterGameSubsystem(pMessages);
	}

	void GameMgr::Release()
	{
		RequestUnloadingAllLevels();
		ExecuteUnloadingRequests();

		delete m_pRenderPassBase;
		delete m_pRenderPassShadowMaps;
		delete m_pRenderPassBloom;
		delete m_pRenderPassUi;
		delete m_pDefaultCamera;
		delete m_pGameContext;

		for (ISubsystem* pSubsystem : m_gameSubsystems)
			delete pSubsystem;

		m_gameSubsystems.Clear();
	}

	void GameMgr::Update(float dt)
	{
		m_pGameContext->m_pClock->Update(dt);

		for (ISubsystem* pSubsystem : m_gameSubsystems)
			pSubsystem->BeforeUpdate(*m_pGameContext);

		for (Systems::LevelAsset* pLevel : m_loadedLevels)
		{
			//going through the list of game object of levels will cause problems when game objects
			//are created at runtime.
			//I should make an internal array of all game object pointers in the GameMgr.
			Core::Array<Systems::GameObject*>& gameObjects = pLevel->GetGameObjectsArray();
			for (Systems::GameObject* pGo : gameObjects)
			{
				pGo->Update(m_pGameContext->m_pClock->GetDeltaTime());
			}
		}

		m_pGameContext->m_pParticleSystem->Update(*m_pGameContext);

		for (ISubsystem* pSubsystem : m_gameSubsystems)
			pSubsystem->Update(*m_pGameContext);

		// Loading/Unloading is synchronous for now. So it blocks the main frame.
		ExecuteLoadingRequests();
		ExecuteUnloadingRequests();
	}

	void GameMgr::Render()
	{
		Systems::RenderableScene scene;

		const Rendering::Camera* pCamera = m_pGameContext->m_pCameraSubsystem->GetTopCamera();
		Systems::PrepareRenderableCamera(pCamera->GetViewMatrix(), pCamera->GetProjectionMatrix(), pCamera->GetPosition(), pCamera->GetFOV(), scene);

		for (Systems::LevelAsset* pLevel : m_loadedLevels)
		{
			Systems::PrepareRenderableScene(pLevel, scene, m_pGameContext->m_pClock->GetTime());
		}

		m_pGameContext->m_pParticleSystem->BuildRenderable(scene);

		for (ISubsystem* pSubsystem : m_gameSubsystems)
			pSubsystem->BuildRenderable(scene);

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

		m_pRenderPassUi->PreRender(scene);
		m_pRenderPassUi->Render(scene);
		m_pRenderPassUi->PostRender(scene);
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

	uint32_t GameMgr::RegisterGameSubsystem(ISubsystem* pSubsystem)
	{
		uint32_t index = m_gameSubsystems.GetSize();
		m_gameSubsystems.PushBack(pSubsystem);
		return index;
	}

	ISubsystem* GameMgr::GetGameSubsystem(uint32_t index)
	{
		return m_gameSubsystems[index];
	}

	const ISubsystem* GameMgr::GetGameSubsystem(uint32_t index) const
	{
		return m_gameSubsystems[index];
	}

	Rendering::RenderTarget* GameMgr::GetFinalRenderTarget()
	{
		return m_pRenderPassBase->GetRenderTarget();
	}

	GameContext* GameMgr::GetWorld()
	{
		return m_pGameContext;
	}

	const GameObject* GameMgr::FindGameObject(Core::Sid gameObjectTypename) const
	{
		for (const LevelAsset* pLevel : m_loadedLevels)
		{
			const Core::Array<GameObject*>& gameObjectArray = pLevel->GetGameObjectsArray();
			for (const GameObject* pGo : gameObjectArray)
			{
				if (pGo->GetTypeDescriptor()->GetSid() == gameObjectTypename)
					return pGo;
			}
		}

		return nullptr;
	}

	const GameComponent* GameMgr::FindComponent(Core::Sid componentTypeName) const
	{
		for (const LevelAsset* pLevel : m_loadedLevels)
		{
			const Core::Array<GameObject*>& gameObjectArray = pLevel->GetGameObjectsArray();
			for (const GameObject* pGo : gameObjectArray)
			{
				const Core::Array<GameComponent*>& componentsArray = pGo->GetComponents();
				for (const GameComponent* pComponent : componentsArray)
				{
					if (pComponent->GetTypeDescriptor()->GetSid() == componentTypeName)
						return pComponent;
				}
			}
		}

		return nullptr;
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

			InstanciateLevel(pLevel, m_pGameContext);

			Core::Array<GameObject*>& objectArray = pLevel->GetGameObjectsArray();
			for (GameObject* pObject : objectArray)
				pObject->OnStartGame();
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

			Core::Array<GameObject*>& objectArray = pLevel->GetGameObjectsArray();
			for (GameObject* pObject : objectArray)
				pObject->OnDestroyGame();

			DeleteInstanciatedLevel(pLevel, m_pGameContext);

			Systems::AssetUtil::UnloadAsset(id, Systems::LoadingDomain::GAME);

			m_loadedLevels.Erase(pLevel);
		}

		m_unloadingRequest.Clear();
	}
}
