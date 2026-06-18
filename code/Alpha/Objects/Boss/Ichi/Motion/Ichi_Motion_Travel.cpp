/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Motion/Ichi_Motion_Travel.h"

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

IchiMotionTravel::IchiMotionTravel(StateMachine* pMachine, Ichi* pIchi)
	: IState(pMachine)
	, m_pIchi(pIchi)
	, m_distanceAcc()
	, m_distanceMaxSpeed()
	, m_distanceDec()
	, m_length2()
	, m_speed()
	, m_maxSpeed(5)
	, m_internalState(InternalState::ACCELERATE)
{ }

void IchiMotionTravel::OnEnter()
{
	Core::Vec4f startPosition = m_pIchi->GetTransform().GetLocalTx().GetT();

	Core::Vec4f direction = m_target - startPosition;
	m_length2 = direction.Length2();
	m_speed = 0;

	//divide the path in 3 steps : accelerate to max speed, travel at max speed, decelerate to 0 speed
	//[0, 15%] : acceleration
	//[15%, 85%] max speed
	//[85%, 100%] decelerate
	m_distanceAcc = m_length2 * 0.15f;
	m_distanceMaxSpeed = m_length2 * 0.85f;
	m_distanceDec = m_distanceAcc;

	m_internalState = InternalState::ACCELERATE;
}

void IchiMotionTravel::OnUpdate()
{
	const float MIN_SPEED = 2.5f;

	const float ROTATION = 45 * Core::PI_OVER_180;

	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	float distanceLeft = (currentPosition - m_target).Length2();

	Core::Vec4f motionDirection = m_target - currentPosition;
	motionDirection.Normalize();

	float distanceAccomplished = m_length2 - distanceLeft;

	switch (m_internalState)
	{
	case ACCELERATE:
	{
		Core::Vec4f rotationAxis = Core::Vec4f(0, 1, 0, 0).Cross(motionDirection);

		if (distanceAccomplished <= m_distanceAcc)
		{
			float param = distanceAccomplished / m_distanceAcc;
			m_speed = ((m_maxSpeed - MIN_SPEED) * param) + MIN_SPEED;

			float rotation = ROTATION * param;
	
			Core::Quaternion quat = Core::Quaternion::FromAxisAngle(rotationAxis, rotation);
			m_pIchi->GetTransform().SetLocalRotation(quat);
		}
		else
		{
			m_internalState = CONSTANTE_SPEED;

			Core::Quaternion quat = Core::Quaternion::FromAxisAngle(rotationAxis, ROTATION);
			m_pIchi->GetTransform().SetLocalRotation(quat);
		}
	}
	break;

	case CONSTANTE_SPEED:
	{
		if (distanceAccomplished >= m_distanceMaxSpeed)
		{
			m_internalState = DECELERATE;
		}
	}
	break;

	case DECELERATE:
	{
		Core::Vec4f rotationAxis = Core::Vec4f(0, 1, 0, 0).Cross(motionDirection);

		if (distanceLeft > 0)
		{
			float param = (distanceAccomplished - m_distanceMaxSpeed) / m_distanceDec;
			param = 1 - param;
			m_speed = ((m_maxSpeed - MIN_SPEED) * param) + MIN_SPEED;
			float rotation = ROTATION * param;

			Core::Quaternion quat = Core::Quaternion::FromAxisAngle(rotationAxis, rotation);
			m_pIchi->GetTransform().SetLocalRotation(quat);
		}
		else
		{
			Core::Quaternion quat = Core::Quaternion::FromAxisAngle(rotationAxis, 0);
			m_pIchi->GetTransform().SetLocalRotation(quat);
			GoTo(IchiMotionState::STOP);
			return;
		}
	}
	break;

	}

	float currentDt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	float newDistance = m_speed * currentDt;

	if (distanceLeft < newDistance)
	{
		m_pIchi->GetTransform().SetLocalTranslation(m_target);
	}
	else
	{
		Core::Vec4f newPosition = currentPosition + motionDirection * m_speed * currentDt;
		m_pIchi->GetTransform().SetLocalTranslation(newPosition);
	}
	
}

void IchiMotionTravel::OnExit()
{
}

void IchiMotionTravel::SetTarget(const Core::Vec4f& target)
{
	m_target = target;
}

void IchiMotionTravel::SetMaxSpeed(float maxSpeed)
{
	m_maxSpeed = maxSpeed;
}
