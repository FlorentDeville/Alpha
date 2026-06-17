/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Dying.h"

#include "Alpha/Objects/Player/PlayerGameObject.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Dying::Ichi_Dying(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_internalState(InternalState::PREPARE)
{ }

Ichi_Dying::~Ichi_Dying()
{ }

void Ichi_Dying::OnEnter()
{
	m_internalState = InternalState::PREPARE;

	m_pIchi->GoToMotionState(IchiMotionState::STOP);
}

void Ichi_Dying::OnUpdate()
{ 
	switch (m_internalState)
	{
	case PREPARE:
	{
		m_pIchi->HideHpBar();

		//lock camera position
		PlayerGameObject* pPlayer = Systems::GameMgr::Get().FindGameObject<PlayerGameObject>();
		pPlayer->SetCameraModeLocked();

		m_internalState = START;
	}
	break;

	case START:
	{
		//start smoke effect
		m_pIchi->StartTransitionEffect();

		//start rumble
		m_rumbleStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		m_rumbleInitialPosition = m_pIchi->GetTransform().GetLocalSqt().GetTranslation();

		m_internalState = LOWER_TOWER;
	}
	break;

	case LOWER_TOWER:
	{
		Rumble();

		//fall	

		m_internalState = MIDDLE_TOWER;
	}
	break;

	case MIDDLE_TOWER:
	{
		Rumble();

		//fall

		m_internalState = UPPER_TOWER;
	}
	break;

	case UPPER_TOWER:
	{
		Rumble();

		//fall

		m_internalState = STOP;
	}
	break;

	case STOP:
	{
		//stop effect
		m_pIchi->StopTransitionEffect();
		m_pIchi->KillEngineEffects();

		//don't rumble anymore

		m_internalState = OVER;
	}
	break;

	case OVER:
	{}
	break;

	}
}

void Ichi_Dying::OnExit()
{ }

void Ichi_Dying::Rumble()
{
	const float RUMBLE_FREQUENCY = 50;
	const float RUMBLE_AMPLITUDE = 0.5f;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	float offset = sinf((currentTime - m_rumbleStartTime) * RUMBLE_FREQUENCY) * RUMBLE_AMPLITUDE;

	Core::Vec4f positionOffset(offset, 0, 0, 0);
	m_pIchi->GetTransform().SetLocalTranslation(m_rumbleInitialPosition + positionOffset);
}