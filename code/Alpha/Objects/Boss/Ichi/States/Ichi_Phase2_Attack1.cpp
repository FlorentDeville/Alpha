/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Attack1.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A1.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase2_Attack1::Ichi_Phase2_Attack1(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{
	m_waveCount = m_pIchi->GetPhase2GunsAttachPointsCount();

	m_pWave = new IchiWaveP1A1*[m_waveCount];
	m_waveIndex = new uint32_t[m_waveCount];

	const uint32_t BULLET_COUNT = 50;
	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
	{
		m_pWave[ii] = new IchiWaveP1A1(m_pIchi->GetBulletMesh(), m_pIchi->GetBulletMaterial(), BULLET_COUNT);
		m_waveIndex[ii] = UINT32_MAX;
	}
}

Ichi_Phase2_Attack1::~Ichi_Phase2_Attack1()
{
	DestroyWaves();

	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
		delete m_pWave[ii];

	delete[] m_pWave;
	m_pWave = nullptr;

	delete[] m_waveIndex;
	m_waveIndex = nullptr;
}

void Ichi_Phase2_Attack1::OnEnter()
{
	UpdateWavesSpawnParameters();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
		pSubsystem->StartWave(m_waveIndex[ii]);
}

void Ichi_Phase2_Attack1::OnUpdate()
{
	{
		const float ROTATION_SPEED = 0.5;
		float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
		float newRotation = ROTATION_SPEED * dt;

		Core::Quaternion quat = m_pIchi->GetTransform().GetLocalSqt().GetRotationQuaternion();
		Core::Quaternion rot = Core::Quaternion::FromEulerAngles(0, newRotation, 0);

		Core::Quaternion finalRot = rot * quat;

		m_pIchi->GetTransform().SetLocalRotation(finalRot);
	}

	UpdateWavesSpawnParameters();
}

void Ichi_Phase2_Attack1::OnExit()
{
	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
		m_pWave[ii]->DisableSpawn();
}

void Ichi_Phase2_Attack1::InitWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
	{
		if (m_waveIndex[ii] != UINT32_MAX)
			continue;

		m_waveIndex[ii] = pSubsystem->AddWave(m_pWave[ii]);
		pSubsystem->InitWave(m_waveIndex[ii]);
	}
}

void Ichi_Phase2_Attack1::DestroyWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
	{
		if (m_waveIndex[ii] == UINT32_MAX)
			continue;

		pSubsystem->DestroyWave(m_waveIndex[ii]);
		pSubsystem->RemoveWave(m_waveIndex[ii]);

		m_waveIndex[ii] = UINT32_MAX;
	}
}

void Ichi_Phase2_Attack1::UpdateWavesSpawnParameters()
{
	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase2GunsAttachPoints();
	const float SPEED = 20;

	const Core::Quaternion& rot = m_pIchi->GetTransform().GetLocalSqt().GetRotationQuaternion();
	Core::Quaternion rot_conj = rot.Conjugate();
	
	Core::Quaternion zAxis(0, 0, 1, 0);
	Core::Quaternion xAxis(1, 0, 0, 0);

	Core::Quaternion qRotateZAxis = rot * zAxis * rot_conj;
	Core::Quaternion qRotateXAxis = rot * xAxis * rot_conj;

	Core::Vec4f rotateZAxis(qRotateZAxis.GetX(), qRotateZAxis.GetY(), qRotateZAxis.GetZ(), 0);
	Core::Vec4f rotateXAxis(qRotateXAxis.GetX(), qRotateXAxis.GetY(), qRotateXAxis.GetZ(), 0);

	rotateZAxis = rotateZAxis * SPEED;
	rotateXAxis = rotateXAxis * SPEED;

	Core::Vec4f negRotateZAxis = rotateZAxis * -1;
	Core::Vec4f negRotateXAxis = rotateXAxis * -1;

	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
	{
		Core::Mat44f wsTx = pGunAttachPoints[ii] * m_pIchi->GetTransform().GetWorldTx();
		m_pWave[ii]->SetSpawnPosition(wsTx.GetT());

		switch (ii)
		{
		case 0:
		case 1:
			m_pWave[ii]->SetSpawnSpeed(rotateZAxis);
			break;

		case 2:
		case 3:
			m_pWave[ii]->SetSpawnSpeed(negRotateXAxis);
			break;

		case 4:
		case 5:
			m_pWave[ii]->SetSpawnSpeed(negRotateZAxis);
			break;

		case 6:
		case 7:
			m_pWave[ii]->SetSpawnSpeed(rotateXAxis);
			break;
		}
	}
}
