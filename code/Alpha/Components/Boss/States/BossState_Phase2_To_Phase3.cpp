/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossState_Phase2_To_Phase3.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/BossGameObject.h"

BossState_Phase2_To_Phase3::BossState_Phase2_To_Phase3(StateMachine* pStateMachine, BossGameObject* pBoss)
	: IState(pStateMachine)
	, m_pBoss(pBoss)
{
}

BossState_Phase2_To_Phase3::~BossState_Phase2_To_Phase3()
{
}

void BossState_Phase2_To_Phase3::OnEnter()
{
	m_pBoss->ExitPhase2();
}

void BossState_Phase2_To_Phase3::OnUpdate()
{
	GoTo(BossStateEnum::PHASE3_TRAVEL);
}

void BossState_Phase2_To_Phase3::OnExit()
{
	m_pBoss->EnterPhase3();
	m_pBoss->SetCurrentHP(m_pBoss->GetMaxHP());
}
