/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Attack1.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase1_Attack1::Ichi_Phase1_Attack1(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{
}

void Ichi_Phase1_Attack1::OnEnter()
{
	m_currentRotation = 0;
	m_rotationSpeed = 0.5f;
}

void Ichi_Phase1_Attack1::OnUpdate()
{
	float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	float newRotation = m_rotationSpeed * dt;

	Core::Quaternion quat = m_pIchi->GetTransform().GetLocalSqt().GetRotationQuaternion();
	Core::Quaternion rot = Core::Quaternion::FromEulerAngles(0, newRotation, 0);

	Core::Quaternion finalRot = rot * quat;

	m_pIchi->GetTransform().SetLocalRotation(finalRot);
}

void Ichi_Phase1_Attack1::OnExit()
{
}
