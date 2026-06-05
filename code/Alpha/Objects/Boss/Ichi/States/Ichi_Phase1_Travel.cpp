/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Travel.h"

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
}

void Ichi_Phase1_Travel::OnUpdate()
{ 
	//const float ACC = 2;
	const float MAX_SPEED = 5;
	const float MIN_SPEED = 0.5f;

	//float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	float distanceLeft = (currentPosition - m_target).Length2();

	if (distanceLeft < 0.01f) //destination reached
	{
		m_pIchi->GetTransform().SetLocalTranslation(m_target);
		return;
	}

	float distanceAccomplished = m_length2 - distanceLeft;
	if (distanceAccomplished <= m_distanceAcc)
	{
		float param = distanceAccomplished / m_distanceAcc;
		m_speed = ((MAX_SPEED - MIN_SPEED) * param) + MIN_SPEED;
	}
	else if (distanceAccomplished >= m_distanceMaxSpeed)
	{
		float param = (distanceAccomplished - m_distanceMaxSpeed) / m_distanceAcc;
		param = 1 - param;
		m_speed = ((MAX_SPEED - MIN_SPEED) * param) + MIN_SPEED;
	}

	float currentDt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	Core::Vec4f motionDirection = m_target - currentPosition;
	motionDirection.Normalize();
	Core::Vec4f newPosition = currentPosition + motionDirection * m_speed * currentDt;
	
	m_pIchi->GetTransform().SetLocalTranslation(newPosition);
}

void Ichi_Phase1_Travel::OnExit()
{ }
