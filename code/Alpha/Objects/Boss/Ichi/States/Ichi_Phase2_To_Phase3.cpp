/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_To_Phase3.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"

Ichi_Phase2_To_Phase3::Ichi_Phase2_To_Phase3(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{ }

Ichi_Phase2_To_Phase3::~Ichi_Phase2_To_Phase3()
{ }

void Ichi_Phase2_To_Phase3::OnEnter()
{
	m_pIchi->ExitPhase2();
}

void Ichi_Phase2_To_Phase3::OnUpdate()
{
	GoTo(IchiStateEnum::PHASE3_TRAVEL);
}

void Ichi_Phase2_To_Phase3::OnExit()
{
	m_pIchi->EnterPhase3();
}
