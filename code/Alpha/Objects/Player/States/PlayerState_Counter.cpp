/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/States/PlayerState_Counter.h"

PlayerState_Counter::PlayerState_Counter(StateMachine* pMachine, PlayerGameObject* pPlayer)
	: IState(pMachine)
	, m_pPlayer(pPlayer)
{ }

void PlayerState_Counter::OnEnter()
{ }

void PlayerState_Counter::OnUpdate()
{ }

void PlayerState_Counter::OnExit()
{ }
