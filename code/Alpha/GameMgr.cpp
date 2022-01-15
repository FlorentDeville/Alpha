/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "GameMgr.h"

#include "Entities/CameraEntity.h"
#include "Entities/GameEntity.h"

void GameMgr::Init()
{}

void GameMgr::Release()
{}

void GameMgr::CreatePlayerEntity()
{}

void GameMgr::CreateCameraEntity()
{}

void GameMgr::CreateBackgroundEntity()
{}

void GameMgr::Update()
{
	for (GameEntity* pEntity : m_entities)
		pEntity->Update();
}

void GameMgr::Render()
{
	for (GameEntity* pEntity : m_entities)
		pEntity->Render();
}

const DirectX::XMMATRIX& GameMgr::GetView() const
{
	return m_pCurrentCamera->GetView();
}

const DirectX::XMMATRIX& GameMgr::GetProjection() const
{
	return m_pCurrentCamera->GetProjection();
}

GameMgr::GameMgr()
	: m_entities()
	, m_pCurrentCamera()
{}

GameMgr::~GameMgr()
{
	for (GameEntity* pEntity : m_entities)
		delete pEntity;

	m_entities.clear();

	m_pCurrentCamera = nullptr;
}
