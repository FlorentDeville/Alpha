/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossStateWait.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

#include <random>

BossStateWait::BossStateWait(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_startTime(0)
	, m_waitTime(0)
{ }

BossStateWait::~BossStateWait()
{ }

void BossStateWait::OnEnter()
{
	Systems::GameMgr& gameMgr = Systems::GameMgr().Get();
	Systems::GameContext* pContext = gameMgr.GetWorld();
	m_startTime = pContext->m_pClock->GetTime();

	//generate a random duration between 1 and 4 seconds
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(1, 4);

	m_waitTime = dis(gen);
}

void BossStateWait::OnUpdate()
{
	Systems::GameMgr& gameMgr = Systems::GameMgr().Get();
	Systems::GameContext* pContext = gameMgr.GetWorld();
	float currentTime = pContext->m_pClock->GetTime();

	if (m_startTime + m_waitTime <= currentTime)
	{
		Core::LogModule::Get().LogInfo("BossStateWait goto state WAVE_TEST");
		GoTo(BossStateEnum::WAVE_TEST);
	}
}

void BossStateWait::OnExit()
{ }
