/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Start.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

#include <assert.h>

Ichi_Start::Ichi_Start(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_state(WAIT)
	, m_pIchi(pIchi)
{
	m_stateWaitParam.m_duration = 2;

	m_stateRumbleParam.m_duration = 5;
	m_stateRumbleParam.m_intensity = 0.05f;
	m_stateRumbleParam.m_frequency = 40;

	m_stateLiftoffParam.m_duration = 0.2f;
	m_stateLiftoffParam.m_distance = 1.5f;

	m_stateOverParam.m_duration = 3;

	m_stateStartTime = 0;
}

void Ichi_Start::OnEnter()
{
	Core::LogModule::Get().LogInfo("Wait");
	m_state = WAIT;
	m_stateStartTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();
}

void Ichi_Start::OnUpdate()
{
	switch (m_state)
	{
	case WAIT:
		Update_Wait();
		break;

	case RUMBLE:
		Update_Rumble();
		break;

	case LIFTOFF:
		Update_Liftoff();
		break;

	case OVER:
		Update_Over();
		break;

	default:
		assert(false && "Unknown state");
		break;
	}
}

void Ichi_Start::OnExit()
{ }

void Ichi_Start::Update_Wait()
{
	float currentTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();
	if (m_stateStartTime + m_stateWaitParam.m_duration <= currentTime)
	{
		Core::LogModule::Get().LogInfo("Rumble");
		m_state = RUMBLE;
		m_stateStartTime = currentTime;
		m_stateRumbleParam.m_initialPosition = m_pIchi->GetTransform().GetLocalSqt().GetTranslation();
	}
}

void Ichi_Start::Update_Rumble()
{
	float currentTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();
	if (m_stateStartTime + m_stateRumbleParam.m_duration <= currentTime)
	{
		m_state = LIFTOFF;
		m_stateStartTime = currentTime;

		Enter_Liftoff();

		return;
	}

	//compute rumble
	float elapsedTime = currentTime - m_stateStartTime;

	float offset = sinf(elapsedTime * m_stateRumbleParam.m_frequency) * m_stateRumbleParam.m_intensity;
	Core::Vec4f offsetVector(offset, 0, 0, 0);

	m_pIchi->GetTransform().SetLocalTranslation(m_stateRumbleParam.m_initialPosition + offsetVector);
}

void Ichi_Start::Enter_Liftoff()
{
	Core::LogModule::Get().LogInfo("Liftoff");
	m_pIchi->GetTransform().SetLocalTranslation(m_stateRumbleParam.m_initialPosition);
	m_stateLiftoffParam.m_initialPosition = m_stateRumbleParam.m_initialPosition;

	//spawn the effects
	m_pIchi->SpawnEngineEffects();
}

void Ichi_Start::Update_Liftoff()
{
	float currentTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();
	if (m_stateStartTime + m_stateLiftoffParam.m_duration <= currentTime)
	{
		
		m_state = OVER;
		m_stateStartTime = currentTime;

		Enter_Over();
		return;
	}

	float elapsedTime = currentTime - m_stateStartTime;
	float param = elapsedTime / m_stateLiftoffParam.m_duration;

	float currentDistance = m_stateLiftoffParam.m_distance * param;
	Core::Vec4f offsetVector(0, currentDistance, 0, 0);
	m_pIchi->GetTransform().SetLocalTranslation(m_stateRumbleParam.m_initialPosition + offsetVector);
}

void Ichi_Start::Enter_Over()
{
	Core::LogModule::Get().LogInfo("Over");
	m_pIchi->GoToMotionState(IchiMotionState::IDLE);
}

void Ichi_Start::Update_Over()
{
	float currentTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();
	if (m_stateStartTime + m_stateOverParam.m_duration <= currentTime)
	{
		m_pIchi->GoToMotionState(IchiMotionState::STOP);
		GoTo(IchiStateEnum::PHASE1_TRAVEL);
		return;
	}
}
