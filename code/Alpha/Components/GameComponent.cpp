/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "GameComponent.h"

GameComponent::GameComponent(GameEntity* pParent)
	: m_pParent(pParent)
{}

GameComponent::~GameComponent()
{}

void GameComponent::Update()
{}

void GameComponent::Render()
{}
