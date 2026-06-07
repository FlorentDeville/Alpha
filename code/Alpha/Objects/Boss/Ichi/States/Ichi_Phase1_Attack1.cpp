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
	, m_warmupDuration(2)
{
	const uint32_t BULLET_COUNT = 110;

	for (uint8_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		m_pWave[ii] = new IchiWaveP1A1(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
		m_waveIndex[ii] = BulletSubsystem::GetSubsystem()->AddWave(m_pWave[ii]);

		//should be initialized only when phase1 is activated
		BulletSubsystem::GetSubsystem()->InitWave(m_waveIndex[ii]);
	}	
}

Ichi_Phase1_Attack1::~Ichi_Phase1_Attack1()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	for (uint8_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		pSubsystem->DestroyWave(m_waveIndex[ii]);
		pSubsystem->RemoveWave(m_waveIndex[ii]);

		delete m_pWave[ii];
		m_pWave[ii] = nullptr;
		m_waveIndex[ii] = UINT32_MAX;
	}
}

void Ichi_Phase1_Attack1::OnEnter()
{
	m_rotationSpeed = 0.5f;

	UpdateWaves();

	for (uint8_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		const Core::Mat44f& ichiWsTx = m_pIchi->GetTransform().GetWorldTx();
		BulletSubsystem::GetSubsystem()->StartWave(m_waveIndex[ii], ichiWsTx.GetT());
	}

	m_warmupStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
}

void Ichi_Phase1_Attack1::OnUpdate()
{
	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	if (m_warmupStartTime + m_warmupDuration <= currentTime)
	{
		float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
		float newRotation = m_rotationSpeed * dt;

		Core::Quaternion quat = m_pIchi->GetTransform().GetLocalSqt().GetRotationQuaternion();
		Core::Quaternion rot = Core::Quaternion::FromEulerAngles(0, newRotation, 0);

		Core::Quaternion finalRot = rot * quat;

		m_pIchi->GetTransform().SetLocalRotation(finalRot);

		UpdateWaves();
	}
}

void Ichi_Phase1_Attack1::OnExit()
{
}

void Ichi_Phase1_Attack1::UpdateWaves()
{
	//update spawn position and speed to match the rotation
	const Core::Mat44f* pAttachPoints = m_pIchi->GetPhase1GunsAttachPoints();
	for (uint8_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		const Core::Mat44f& ichiWsTx = m_pIchi->GetTransform().GetWorldTx();
		Core::Mat44f finalTx = pAttachPoints[ii] * ichiWsTx;
		m_pWave[ii]->SetSpawnPosition(finalTx.GetT());

		Core::Vec4f velocity = finalTx.GetT() - ichiWsTx.GetT();
		velocity.Set(1, 0);
		velocity.Normalize();
		
		const float SPEED = 5;
		velocity = velocity * SPEED;

		m_pWave[ii]->SetSpawnSpeed(velocity);
	}
}