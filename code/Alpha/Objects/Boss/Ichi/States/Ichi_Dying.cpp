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
	, m_internlStateStartTime(0)
{ }

Ichi_Dying::~Ichi_Dying()
{ }

void Ichi_Dying::OnEnter()
{
	GoToInternalState(PREPARE);
	
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

		GoToInternalState(START);
	}
	break;

	case START:
	{
		//start smoke effect
		m_pIchi->StartTransitionEffect();

		//start rumble
		m_rumbleStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		m_rumbleInitialPosition = m_pIchi->GetTransform().GetLocalSqt().GetTranslation();

		GoToInternalState(LOWER_TOWER);
	}
	break;

	case LOWER_TOWER:
	{
		Rumble();

		//fall	

		GoToInternalState(MIDDLE_TOWER);
	}
	break;

	case MIDDLE_TOWER:
	{
		Rumble();

		//fall

		GoToInternalState(UPPER_TOWER);
	}
	break;

	case UPPER_TOWER:
	{
		Rumble();

		//fall

		GoToInternalState(STOP);
	}
	break;

	case STOP:
	{
		//stop effect
		m_pIchi->StopTransitionEffect();
		m_pIchi->KillEngineEffects();

		//don't rumble anymore

		GoToInternalState(OVER);
	}
	break;

	case OVER:
	{}
	break;

	}
}

void Ichi_Dying::OnExit()
{ }

void Ichi_Dying::GoToInternalState(InternalState nextState)
{
	m_internalState = nextState;
	m_internlStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
}

void Ichi_Dying::Rumble()
{
	const float RUMBLE_FREQUENCY = 50;
	const float RUMBLE_AMPLITUDE = 0.5f;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	float offset = sinf((currentTime - m_rumbleStartTime) * RUMBLE_FREQUENCY) * RUMBLE_AMPLITUDE;

	Core::Vec4f positionOffset(offset, 0, 0, 0);
	m_pIchi->GetTransform().SetLocalTranslation(m_rumbleInitialPosition + positionOffset);
}