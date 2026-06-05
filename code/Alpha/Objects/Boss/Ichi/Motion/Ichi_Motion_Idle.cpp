/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Motion/Ichi_Motion_Idle.h"

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

IchiMotionIdle::IchiMotionIdle(StateMachine* pMachine, Ichi* pIchi)
	: IState(pMachine)
	, m_pIchi(pIchi)
	, m_initialPosition()
	, m_amplitude(0.5f)
	, m_frequency(2)
	, m_startTime()
{ }

void IchiMotionIdle::OnEnter()
{
	m_startTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();
	m_initialPosition = m_pIchi->GetTransform().GetLocalTx().GetT();

	ComputeBounce();
}

void IchiMotionIdle::OnUpdate()
{
	ComputeBounce();
}

void IchiMotionIdle::OnExit()
{ }

void IchiMotionIdle::ComputeBounce()
{
	Systems::IClockSubsystem* pClock = Systems::GameMgr::Get().GetWorld()->m_pClock;

	float value = sinf((pClock->GetTime() - m_startTime) * m_frequency) * m_amplitude;
	Core::Vec4f offset(0, value, 0, 0);

	m_pIchi->GetTransform().SetLocalTranslation(m_initialPosition + offset);
}
