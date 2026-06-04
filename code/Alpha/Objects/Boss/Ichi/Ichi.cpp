/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

Ichi::Ichi()
	: BaseClass()
{ }

Ichi::~Ichi()
{ }

void Ichi::OnStartGame()
{
	BaseClass::OnStartGame();
}

void Ichi::Update(float dt)
{
	BaseClass::Update(dt);
}

void Ichi::HandleMessage(const Systems::GameMessage& msg)
{
	BaseClass::HandleMessage(msg);
}

void Ichi::OnDestroyGame()
{
	BaseClass::OnDestroyGame();
}

void Ichi::SetCurrentHP(int32_t hp)
{
	BaseClass::SetCurrentHP(hp);
}
