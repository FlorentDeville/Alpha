/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Travel.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase1_Travel::Ichi_Phase1_Travel(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{ }

void Ichi_Phase1_Travel::OnEnter()
{
	//compute final point
	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	m_target = currentPosition + Core::Vec4f(20, 0, 0, 0);

	Core::Vec4f motionDirection = m_target - currentPosition;
	m_length2 = motionDirection.Length2();
	m_halfLength2 = m_length2 * 0.5f;
	m_speed = 0;

	m_startTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	//divide the path in 3 steps : accelerate to max speed, travel at max speed, decelerate to 0 speed
	m_distanceAcc = m_length2 * 0.25f;
	m_distanceMaxSpeed = m_distanceAcc * 3.5f;
	m_distanceDec = m_length2 - m_distanceMaxSpeed;
}

void Ichi_Phase1_Travel::OnUpdate()
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
		return;
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
		Core::LogModule::Get().LogInfo("param %f", param);
		float rotation = ROTATION * param;
		Core::Vec4f rotationAxis = Core::Vec4f(0, 1, 0, 0).Cross(motionDirection);
		Core::Quaternion quat = Core::Quaternion::FromAxisAngle(rotationAxis, rotation);
		m_pIchi->GetTransform().SetLocalRotation(quat);
	}

	float currentDt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	
	Core::Vec4f newPosition = currentPosition + motionDirection * m_speed * currentDt;
	
	m_pIchi->GetTransform().SetLocalTranslation(newPosition);
}

void Ichi_Phase1_Travel::OnExit()
{ }
