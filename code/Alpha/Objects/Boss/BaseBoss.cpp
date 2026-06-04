/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/BaseBoss.h"

#include "Alpha/StateMachine/StateMachine.h"

BaseBoss::BaseBoss()
	: BaseClass()
	, m_maxHP(100)
	, m_currentHP(100)
	, m_pStateMachine(nullptr)
{ }

BaseBoss::~BaseBoss()
{
	delete m_pStateMachine;
	m_pStateMachine = nullptr;
}

void BaseBoss::OnStartGame()
{
	BaseClass::OnStartGame();

	m_currentHP = m_maxHP;
}

void BaseBoss::Update(float dt)
{
	BaseClass::Update(dt);
}

void BaseBoss::HandleMessage(const Systems::GameMessage& msg)
{
	BaseClass::HandleMessage(msg);
}

void BaseBoss::OnDestroyGame()
{
	BaseClass::OnDestroyGame();
}

uint32_t BaseBoss::GetMaxHP() const
{
	return m_maxHP;
}

int32_t BaseBoss::GetCurrentHP() const
{
	return m_currentHP;
}

void BaseBoss::SetCurrentHP(int32_t hp)
{
	m_currentHP = hp;
}
