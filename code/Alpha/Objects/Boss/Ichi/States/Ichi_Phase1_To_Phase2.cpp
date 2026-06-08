/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_To_Phase2.h"

Ichi_Phase1_To_Phase2::Ichi_Phase1_To_Phase2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{ }

Ichi_Phase1_To_Phase2::~Ichi_Phase1_To_Phase2()
{ }

void Ichi_Phase1_To_Phase2::OnEnter()
{
	m_pIchi->ExitPhase1();

	//display phase 2 mesh
}

void Ichi_Phase1_To_Phase2::OnUpdate()
{ }

void Ichi_Phase1_To_Phase2::OnExit()
{ }
