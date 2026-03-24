/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "GameMgr.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"
#include "Systems/Objects/GameObject.h"

GameMgr::GameMgr()
{ }

GameMgr::~GameMgr()
{ }

void GameMgr::Init()
{ }

void GameMgr::Release()
{ }

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