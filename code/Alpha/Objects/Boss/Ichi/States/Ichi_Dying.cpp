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
	, m_rumbleStartTime(0)
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

		GoToInternalState(PAUSE);
	}
	break;

	case PAUSE:
	{
		Rumble();

		const float DURATION = 3;
		if (IsElaspedTime(DURATION))
			GoToInternalState(LOWER_TOWER);
	}
	break;

	case LOWER_TOWER:
	{
		Rumble();

		//fall	
		Systems::RenderableComponent* pMesh = m_pIchi->GetPhase3Renderable();

		const float SPEED = -4;
		const float ROTATION_SPEED = 0.1f;
		Fall(pMesh, SPEED, ROTATION_SPEED);

		const float DURATION = 4;
		if (IsElaspedTime(DURATION))
		{
			pMesh->SetEnabled(false);
			GoToInternalState(MIDDLE_TOWER);
		}
	}
	break;

	case MIDDLE_TOWER:
	{
		Rumble();

		//fall
		Systems::RenderableComponent* pMesh = m_pIchi->GetPhase2Renderable();

		const float SPEED = -4;
		const float ROTATION_SPEED = -0.2f;
		Fall(pMesh, SPEED, ROTATION_SPEED);

		const float DURATION = 4;
		if (IsElaspedTime(DURATION))
		{
			pMesh->SetEnabled(false);

			//stop effects
			m_pIchi->StopTransitionEffect();
			m_pIchi->KillEngineEffects();

			GoToInternalState(UPPER_TOWER);
		}
	}
	break;

	case UPPER_TOWER:
	{
		Rumble();

		//fall
		Systems::RenderableComponent* pMesh = m_pIchi->GetPhase1Renderable();

		const float SPEED = -5;
		const float ROTATION_SPEED = -0.3f;
		Fall(pMesh, SPEED, ROTATION_SPEED);

		const float DURATION = 4;
		if (IsElaspedTime(DURATION))
		{
			pMesh->SetEnabled(false);
			GoToInternalState(STOP);
		}
	}
	break;

	case STOP:
	{
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

void Ichi_Dying::Fall(Systems::RenderableComponent* pMesh, float fallingSpeed, float rotationSpeed) const
{
	float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();

	Core::Vec4f newPosition = pMesh->GetLocalTx().GetTranslation() + Core::Vec4f(0, fallingSpeed, 0, 0) * dt;
	pMesh->SetLocalTranslation(newPosition);

	Core::Quaternion rotation = Core::Quaternion::FromEulerAngles(0, 0, rotationSpeed * dt);
	Core::Quaternion newRotation = rotation * pMesh->GetLocalTx().GetRotationQuaternion();
	pMesh->SetLocalRotation(newRotation);
}

bool Ichi_Dying::IsElaspedTime(float maxDuration) const
{
	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	if (m_internlStateStartTime + maxDuration <= currentTime)
		return true;

	return false;
}