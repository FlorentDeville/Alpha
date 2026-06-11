/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_To_Phase2.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase1_To_Phase2::Ichi_Phase1_To_Phase2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_internalState(DELAY)
	, m_internalStateStartTime()
	, m_rumbleStartTime()
{ }

Ichi_Phase1_To_Phase2::~Ichi_Phase1_To_Phase2()
{ }

void Ichi_Phase1_To_Phase2::OnEnter()
{
	m_pIchi->GoToMotionState(IchiMotionState::STOP);

	m_initialOrientation = m_pIchi->GetTransform().GetLocalSqt().GetRotationQuaternion();

	float cosAngleOverTwo = m_initialOrientation.GetW();

	if (cosAngleOverTwo > 0.999f)
		m_internalState = DELAY;
	else
		m_internalState = REST_POSITION;

	m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
}

void Ichi_Phase1_To_Phase2::OnUpdate()
{
	switch (m_internalState)
	{
	case REST_POSITION:
	{
		const float DURATION = 2.f;
		const Core::Quaternion dest(0, 0, 0, 1);
		
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		float paramT = (currentTime - m_internalStateStartTime) / DURATION;
		
		Core::Quaternion rot = Core::Quaternion::Slerp(m_initialOrientation, dest, paramT);

		if (paramT >= 1)
		{
			rot = dest;
			m_internalState = DELAY;
			m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		}

		m_pIchi->GetTransform().SetLocalRotation(rot);
	}
	break;

	case DELAY:
	{
		const float DURATION = 0.5f;
		if (ElapsedTime(m_internalStateStartTime, DURATION))
		{
			m_internalState = RUMBLE;
			m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
			m_rumbleStartTime = m_internalStateStartTime;
			m_rumbleStateInitialPosition = m_pIchi->GetTransform().GetLocalSqt().GetTranslation();
		}
	}
	break;

	case RUMBLE:
	{
		const float DURATION = 2;

		Rumble();

		if (ElapsedTime(m_internalStateStartTime, DURATION))
		{
			m_internalState = SMOKE;
			m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

			//spawn smoke
			m_pIchi->StartTransitionEffect();
		}
	}
	break;

	case SMOKE:
	{
		const float DURATION = 2;

		Rumble();

		if (ElapsedTime(m_internalStateStartTime, DURATION))
		{
			m_internalState = DELAY2;
			m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

			m_pIchi->StopTransitionEffect();
			m_pIchi->ShowPhase2Meshes();
		}
	}
	break;

	case STOP_RUMBLE:
	{
		Rumble();

		const float DURATION = 0.5f;
		if (ElapsedTime(m_internalStateStartTime, DURATION))
		{
			m_internalState = RUMBLE;
			m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		}
	}
	break;

	case DELAY2:
	{
		const float DURATION = 0.5f;
		if (ElapsedTime(m_internalStateStartTime, DURATION))
		{
			GoTo(IchiStateEnum::PHASE2_TRAVEL);
		}
	}
	break;

	}
}

void Ichi_Phase1_To_Phase2::OnExit()
{
	m_pIchi->ExitPhase1();
	m_pIchi->EnterPhase2();
}

bool Ichi_Phase1_To_Phase2::ElapsedTime(float start, float duration) const
{
	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	return start + duration < currentTime;
}

void Ichi_Phase1_To_Phase2::Rumble()
{
	const float RUMBLE_FREQUENCY = 50;
	const float RUMBLE_AMPLITUDE = 0.5f;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	float offset = sinf((currentTime - m_rumbleStartTime) * RUMBLE_FREQUENCY) * RUMBLE_AMPLITUDE;

	Core::Vec4f positionOffset(offset, 0, 0, 0);
	m_pIchi->GetTransform().SetLocalTranslation(m_rumbleStateInitialPosition + positionOffset);
}