/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Motion/Ichi_Motion_Strafe.h"

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

IchiMotionStrafe::IchiMotionStrafe(StateMachine* pMachine, Ichi* pIchi)
	: IState(pMachine)
	, m_pIchi(pIchi)
	, m_speed()
{ }

void IchiMotionStrafe::OnEnter()
{ }

void IchiMotionStrafe::OnUpdate()
{
	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	float distanceLeft = (currentPosition - m_target).Length2();

	if (distanceLeft < 0.01f) //destination reached
	{
		m_pIchi->GetTransform().SetLocalTranslation(m_target);
		GoTo(IchiMotionState::STOP);
		return;
	}

	Core::Vec4f motionDirection = m_target - currentPosition;
	motionDirection.Normalize();

	float currentDt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();

	Core::Vec4f newPosition = currentPosition + motionDirection * m_speed * currentDt;

	m_pIchi->GetTransform().SetLocalTranslation(newPosition);
}

void IchiMotionStrafe::OnExit()
{ }

void IchiMotionStrafe::SetTarget(const Core::Vec4f& target)
{
	m_target = target;
}

void IchiMotionStrafe::SetSpeed(float speed)
{
	m_speed = speed;
}