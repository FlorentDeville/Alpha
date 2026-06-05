/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Motion/Ichi_Motion_Travel.h"

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

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
{
}

void IchiMotionTravel::OnEnter()
{
	Core::Vec4f startPosition = m_pIchi->GetTransform().GetLocalTx().GetT();

	Core::Vec4f direction = m_target - startPosition;
	m_length2 = direction.Length2();
	m_speed = 0;

	//divide the path in 3 steps : accelerate to max speed, travel at max speed, decelerate to 0 speed
	m_distanceAcc = m_length2 * 0.25f;
	m_distanceMaxSpeed = m_distanceAcc * 3.5f;
	m_distanceDec = m_length2 - m_distanceMaxSpeed;
}

void IchiMotionTravel::OnUpdate()
{
	const float MAX_SPEED = 5;
	const float MIN_SPEED = 2.5f;

	const float ROTATION = 45 * Core::PI_OVER_180;

	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	float distanceLeft = (currentPosition - m_target).Length2();

	if (distanceLeft < 0.01f) //destination reached
	{
		m_pIchi->GetTransform().SetLocalTranslation(m_target);
		m_pIchi->GetTransform().SetLocalRotation(Core::Quaternion());
		GoTo(IchiMotionState::STOP);
	}

	Core::Vec4f motionDirection = m_target - currentPosition;
	motionDirection.Normalize();

	float distanceAccomplished = m_length2 - distanceLeft;
	if (distanceAccomplished <= m_distanceAcc)
	{
		float param = distanceAccomplished / m_distanceAcc;
		m_speed = ((MAX_SPEED - MIN_SPEED) * param) + MIN_SPEED;

		float rotation = ROTATION * param;

		Core::Vec4f rotationAxis = Core::Vec4f(0, 1, 0, 0).Cross(motionDirection);
		Core::Quaternion quat = Core::Quaternion::FromAxisAngle(rotationAxis, rotation);
		m_pIchi->GetTransform().SetLocalRotation(quat);

	}
	else if (distanceAccomplished >= m_distanceMaxSpeed)
	{
		float param = (distanceAccomplished - m_distanceMaxSpeed) / m_distanceDec;
		param = 1 - param;
		m_speed = ((MAX_SPEED - MIN_SPEED) * param) + MIN_SPEED;
		//Core::LogModule::Get().LogInfo("param %f", param);
		float rotation = ROTATION * param;
		Core::Vec4f rotationAxis = Core::Vec4f(0, 1, 0, 0).Cross(motionDirection);
		Core::Quaternion quat = Core::Quaternion::FromAxisAngle(rotationAxis, rotation);
		m_pIchi->GetTransform().SetLocalRotation(quat);
	}

	float currentDt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();

	Core::Vec4f newPosition = currentPosition + motionDirection * m_speed * currentDt;

	m_pIchi->GetTransform().SetLocalTranslation(newPosition);
}

void IchiMotionTravel::OnExit()
{
}

void IchiMotionTravel::SetTarget(const Core::Vec4f& target)
{
	m_target = target;
}
