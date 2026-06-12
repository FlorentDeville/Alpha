/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_To_Phase3.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase2_To_Phase3::Ichi_Phase2_To_Phase3(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_internalState(DELAY)
	, m_internalStateStartTime()
	, m_rumbleStartTime()
{
	m_pP2Renderable = m_pIchi->GetPhase2Renderable();
}

Ichi_Phase2_To_Phase3::~Ichi_Phase2_To_Phase3()
{
}

void Ichi_Phase2_To_Phase3::OnEnter()
{
	m_pIchi->GoToMotionState(IchiMotionState::STOP);

	m_initialOrientation = m_pIchi->GetTransform().GetLocalSqt().GetRotationQuaternion();
	m_middleTowerInitialOritentation = m_pP2Renderable->GetLocalTx().GetRotationQuaternion();

	m_internalState = REST_POSITION;
	m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	m_isGameObjectInRestPosition = false;
	m_isMiddleTowerInRestPosition = false;
}

void Ichi_Phase2_To_Phase3::OnUpdate()
{
	switch (m_internalState)
	{
	case REST_POSITION:
	{
		if (!m_isGameObjectInRestPosition)
		{
			const float DURATION = 2.f;
			const Core::Quaternion dest(0, 0, 0, 1);

			float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
			float paramT = (currentTime - m_internalStateStartTime) / DURATION;

			Core::Quaternion rot = Core::Quaternion::Slerp(m_initialOrientation, dest, paramT);

			if (paramT >= 1)
			{
				rot = dest;
				m_isGameObjectInRestPosition = true;
			}

			m_pIchi->GetTransform().SetLocalRotation(rot);
		}

		if (!m_isMiddleTowerInRestPosition)
		{
			const float DURATION = 0.75f;

			Core::Quaternion restingRotation(0, 0, 0, 1);
			float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
			float paramT = (currentTime - m_internalStateStartTime) / DURATION;
			if (paramT < 1)
			{
				Core::Quaternion rot = Core::Quaternion::Slerp(m_middleTowerInitialOritentation, restingRotation, paramT);
				m_pP2Renderable->SetLocalRotation(rot);
			}
			else
			{
				m_pP2Renderable->SetLocalRotation(Core::Quaternion(0, 0, 0, 1));
				m_isMiddleTowerInRestPosition = true;
			}
		}

		if (m_isGameObjectInRestPosition && m_isMiddleTowerInRestPosition)
		{
			m_internalState = DELAY;
			m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		}
	}
	break;

	case DELAY:
	{
		const float DURATION = 2.f;
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
		const float DURATION = 3;

		Rumble();

		if (ElapsedTime(m_internalStateStartTime, DURATION))
		{
			m_internalState = DELAY2;
			m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

			m_pIchi->StopTransitionEffect();
			m_pIchi->ShowPhase3Meshes();
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
		const float DURATION = 3.f;
		if (ElapsedTime(m_internalStateStartTime, DURATION))
		{
			GoTo(IchiStateEnum::PHASE3_TRAVEL);
		}
	}
	break;

	}
}

void Ichi_Phase2_To_Phase3::OnExit()
{
	m_pIchi->ExitPhase2();
	m_pIchi->EnterPhase3();
}

bool Ichi_Phase2_To_Phase3::ElapsedTime(float start, float duration) const
{
	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	return start + duration < currentTime;
}

void Ichi_Phase2_To_Phase3::Rumble()
{
	const float RUMBLE_FREQUENCY = 50;
	const float RUMBLE_AMPLITUDE = 0.5f;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	float offset = sinf((currentTime - m_rumbleStartTime) * RUMBLE_FREQUENCY) * RUMBLE_AMPLITUDE;

	Core::Vec4f positionOffset(offset, 0, 0, 0);
	m_pIchi->GetTransform().SetLocalTranslation(m_rumbleStateInitialPosition + positionOffset);
}