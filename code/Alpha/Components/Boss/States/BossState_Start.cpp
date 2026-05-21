/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossState_Start.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/BossGameObject.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

BossState_Start::BossState_Start(StateMachine* pStateMachine, BossGameObject* pBoss)
	: IState(pStateMachine)
	, m_pBoss(pBoss)
{ }

BossState_Start::~BossState_Start()
{ }

void BossState_Start::OnEnter()
{
	m_startTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
}

void BossState_Start::OnUpdate()
{
	const float DURATION = 2;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	if (currentTime >= m_startTime + DURATION)
		GoTo(BossStateEnum::PHASE1_TRAVEL);
}

void BossState_Start::OnExit()
{
	m_pBoss->EnterPhase1();
}
