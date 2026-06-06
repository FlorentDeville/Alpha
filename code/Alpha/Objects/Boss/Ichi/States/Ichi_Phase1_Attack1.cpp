/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Attack1.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A1.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase1_Attack1::Ichi_Phase1_Attack1(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_rotationSpeed(0)
{
	const uint32_t BULLET_COUNT = 50;
	m_pWave = new IchiWaveP1A1(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
	m_waveIndex = BulletSubsystem::GetSubsystem()->AddWave(m_pWave);

	BulletSubsystem::GetSubsystem()->InitWave(m_waveIndex);
}

Ichi_Phase1_Attack1::~Ichi_Phase1_Attack1()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->DestroyWave(m_waveIndex);
	pSubsystem->RemoveWave(m_waveIndex);

	delete m_pWave;
	m_pWave = nullptr;
	m_waveIndex = UINT32_MAX;
}

void Ichi_Phase1_Attack1::OnEnter()
{
	m_rotationSpeed = 0.5f;

	BulletSubsystem::GetSubsystem()->StartWave(m_waveIndex, m_pIchi->GetTransform().GetWorldTx().GetT());
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
