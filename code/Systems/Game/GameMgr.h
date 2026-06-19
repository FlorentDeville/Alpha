/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Reflection/TypeResolver.h"
#include "Core/Sid/Sid.h"
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
	class IClockSubsystem;
	class ISubsystem;
	class GameComponent;
	class LevelAsset;
	class ParticleSystem;
	class RenderPassBase;
	class RenderPassBloom;
	class RenderPassShadowMaps;
	class RenderPassUI;
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
		ISubsystem* GetGameSubsystem(uint32_t index);
		const ISubsystem* GetGameSubsystem(uint32_t index) const;

		Rendering::RenderTarget* GetFinalRenderTarget();

		GameContext* GetWorld();

		template<class T> const T* FindGameObject() const;
		const GameObject* FindGameObject(Core::Sid gameObjectTypename) const;

		template<class T> T* FindGameObject();
		GameObject* FindGameObject(Core::Sid gameObjectTypename);

		//!!!!Slow!!!!
		//I don't like this function. Once I have a property grid with scrolling and game objet ref, remove this function.
		GameObject* FindGameObjectByName(const std::string& objectName);

		template<class T> const T* FindComponent() const;
		const GameComponent* FindComponent(Core::Sid componentTypeName) const;

		bool Debug_ShowCollision() const;
		void Debug_SetShowCollision(bool show);

		static IClockSubsystem* GetClock();
		static ParticleSystem* GetParticleSystem();

	private:
		//Loading
		Core::Array<Systems::LevelAsset*> m_loadedLevels;		//pointers to the currently loaded levels.

		Core::Array<Systems::NewAssetId> m_loadingRequest;
		Core::Array<Systems::NewAssetId> m_unloadingRequest;

		//Rendering
		Systems::RenderPassBase* m_pRenderPassBase;
		Systems::RenderPassShadowMaps* m_pRenderPassShadowMaps;
		Systems::RenderPassBloom* m_pRenderPassBloom;
		Systems::RenderPassUI* m_pRenderPassUi;

		//Camera
		Rendering::Camera* m_pDefaultCamera;

		GameContext* m_pGameContext;

		Core::Array<ISubsystem*> m_gameSubsystems;

		bool m_debugShowCollision;

		bool IsLevelAlreadyLoaded(Systems::NewAssetId id) const;

		void ExecuteLoadingRequests();
		void ExecuteUnloadingRequests();
	};

	template<class T> const T* GameMgr::FindGameObject() const
	{
		return static_cast<const T*>(FindGameObject(Core::TypeResolver<T>::GetTypenameSid()));
	}

	template<class T> T* GameMgr::FindGameObject()
	{
		return static_cast<T*>(FindGameObject(Core::TypeResolver<T>::GetTypenameSid()));
	}

	template<class T> const T* GameMgr::FindComponent() const
	{
		return static_cast<const T*>(FindComponent(Core::TypeResolver<T>::GetTypenameSid()));
	}
}
