/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/States/BossState_Phase1_To_Phase2.h"

#include "Alpha/Objects/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/Boss/BossGameObject.h"

BossState_Phase1_To_Phase2::BossState_Phase1_To_Phase2(StateMachine* pStateMachine, BossGameObject* pBoss)
	: IState(pStateMachine)
	, m_pBoss(pBoss)
{ }

BossState_Phase1_To_Phase2::~BossState_Phase1_To_Phase2()
{ }

void BossState_Phase1_To_Phase2::OnEnter()
{
	m_pBoss->ExitPhase1();
}

void BossState_Phase1_To_Phase2::OnUpdate()
{
	GoTo(BossStateEnum::PHASE2_TRAVEL);
}

void BossState_Phase1_To_Phase2::OnExit()
{
	m_pBoss->EnterPhase2();
	m_pBoss->SetCurrentHP(m_pBoss->GetMaxHP());
}
