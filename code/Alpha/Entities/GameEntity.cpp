/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "GameEntity.h"

#include "Components/GameComponent.h"

GameEntity::GameEntity()
	: m_components()
{}

GameEntity::~GameEntity()
{
	for (GameComponent* pComponent : m_components)
		delete pComponent;

	m_components.clear();
}

void GameEntity::AddGameComponent(GameComponent* pComponent)
{
	m_components.push_back(pComponent);
}

void GameEntity::Update()
{
	for (GameComponent* pComponent : m_components)
		pComponent->Update();
}

void GameEntity::Render()
{
	for (GameComponent* pComponent : m_components)
		pComponent->Render();
}
