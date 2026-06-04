/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

#include "Alpha/StateMachine/StateMachine.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Start.h"

Ichi::Ichi()
	: BaseClass()
{ }

Ichi::~Ichi()
{ }

void Ichi::OnStartGame()
{
	BaseClass::OnStartGame();

	m_pStateMachine = new StateMachine();
	m_pStateMachine->Init(IchiStateEnum::COUNT);

	m_pStateMachine->AddState(new Ichi_Start(m_pStateMachine), IchiStateEnum::START);

	m_pStateMachine->GoTo(IchiStateEnum::START);
}

void Ichi::Update(float dt)
{
	BaseClass::Update(dt);

	m_pStateMachine->Update();
}

void Ichi::HandleMessage(const Systems::GameMessage& msg)
{
	BaseClass::HandleMessage(msg);
}

void Ichi::OnDestroyGame()
{
	BaseClass::OnDestroyGame();

	delete m_pStateMachine;
	m_pStateMachine = nullptr;
}

void Ichi::SetCurrentHP(int32_t hp)
{
	BaseClass::SetCurrentHP(hp);
}
