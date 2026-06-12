/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Attack1.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A1.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P2_A1.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase2_Attack1::Ichi_Phase2_Attack1(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_startTime(0)
	, m_internalState()
{
	m_waveCount = m_pIchi->GetPhase2GunsAttachPointsCount() / 2;

	m_pWave = new IchiWaveP2A1*[m_waveCount];
	m_waveIndex = new uint32_t[m_waveCount];

	const uint32_t BULLET_COUNT = 50;
	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
	{
		m_pWave[ii] = new IchiWaveP2A1(m_pIchi->GetBulletMesh(), m_pIchi->GetBulletMaterial(), BULLET_COUNT);
		m_waveIndex[ii] = UINT32_MAX;
	}

	for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
	{
		m_pUpperGunsWaves[ii] = new IchiWaveP1A1(m_pIchi->GetBulletMesh(), m_pIchi->GetBulletMaterial(), BULLET_COUNT);
		m_upperGunsWaveIndex[ii] = UINT32_MAX;
	}

	m_pP1Renderable = m_pIchi->GetPhase1Renderable();
	m_pP2Renderable = m_pIchi->GetPhase2Renderable();
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

	for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
		delete m_pUpperGunsWaves[ii];
}

void Ichi_Phase2_Attack1::OnEnter()
{
	UpdateWavesSpawnParameters();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
		pSubsystem->StartWave(m_waveIndex[ii]);

	for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
		pSubsystem->StartWave(m_upperGunsWaveIndex[ii]);

	m_startTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();

	m_pIchi->GoToMotionState(IchiMotionState::IDLE);

	m_internalState = WARMUP;
}

void Ichi_Phase2_Attack1::OnUpdate()
{
	const float WARMUP_TIME = 2;
	const float SHOOTING_TIME = 9;

	float currentTime = Systems::GameMgr().Get().GetWorld()->m_pClock->GetTime();

	switch (m_internalState)
	{
	case InternalState::WARMUP:
	{
		if (m_startTime + WARMUP_TIME < currentTime)
			m_internalState = SHOOT;
	}
	break;

	case InternalState::SHOOT:
	{
		UpdateMotion();
		if (m_startTime + SHOOTING_TIME < currentTime)
		{
			for (uint8_t ii = 0; ii < m_waveCount; ++ii)
				m_pWave[ii]->Stop();

			for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
				m_pUpperGunsWaves[ii]->Stop();

			m_internalState = FINISH;
		}
	}
	break;

	case InternalState::FINISH:
	{
		UpdateMotion();

		const Core::Quaternion& rot = m_pP2Renderable->GetLocalTx().GetRotationQuaternion();
		

		float cosAngleOverTwo = abs(rot.GetW());
		const float CLOSE_VALUE = 0.9999f;

		if (cosAngleOverTwo > CLOSE_VALUE)
		{
			Core::Quaternion rest(0, 0, 0, 1);
			m_pP2Renderable->SetLocalRotation(rest);
			GoTo(IchiStateEnum::PHASE2_TRAVEL);
		}
	}
	break;

	}

	UpdateWavesSpawnParameters();
}

void Ichi_Phase2_Attack1::OnExit()
{
	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
		m_pWave[ii]->Stop();

	for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
		m_pUpperGunsWaves[ii]->Stop();
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

	for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
	{
		if (m_upperGunsWaveIndex[ii] != UINT32_MAX)
			continue;

		m_upperGunsWaveIndex[ii] = pSubsystem->AddWave(m_pUpperGunsWaves[ii]);
		pSubsystem->InitWave(m_upperGunsWaveIndex[ii]);
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

	for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
	{
		if (m_upperGunsWaveIndex[ii] == UINT32_MAX)
			continue;

		pSubsystem->DestroyWave(m_upperGunsWaveIndex[ii]);
		pSubsystem->RemoveWave(m_upperGunsWaveIndex[ii]);
	}
}

void Ichi_Phase2_Attack1::UpdateWavesSpawnParameters()
{
	UpdateMiddleGunWavesParameters();
	UpdateUpperGunWavesParameters();
}

void Ichi_Phase2_Attack1::UpdateMotion()
{
	const float ROTATION_SPEED = 0.5;
	float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	float newRotation = ROTATION_SPEED * dt;

	//rotate middle tower
	{
		Core::Quaternion quat = m_pP2Renderable->GetLocalTx().GetRotationQuaternion();
		Core::Quaternion rot = Core::Quaternion::FromEulerAngles(0, newRotation, 0);

		Core::Quaternion finalRot = rot * quat;

		m_pP2Renderable->SetLocalRotation(finalRot);
	}

	//rotate upper tower
	{
		Core::Quaternion quat = m_pP1Renderable->GetLocalTx().GetRotationQuaternion();
		Core::Quaternion rot = Core::Quaternion::FromEulerAngles(0, -newRotation, 0);

		Core::Quaternion finalRot = rot * quat;

		m_pP1Renderable->SetLocalRotation(finalRot);
	}
}

void Ichi_Phase2_Attack1::UpdateUpperGunWavesParameters()
{
	const Core::Mat44f* pAp = m_pIchi->GetPhase1GunsAttachPoints();

	const Core::Mat44f& wsTx = m_pP1Renderable->GetLocalTx().GetMatrix() * m_pIchi->GetTransform().GetWorldTx();

	for (uint8_t ii = 0; ii < UPPER_GUNS_COUNT; ++ii)
	{
		Core::Mat44f apWs = pAp[ii] * wsTx;
		m_pUpperGunsWaves[ii]->SetSpawnPosition(apWs.GetT());

		Core::Vec4f speed = apWs.GetT() - wsTx.GetT();
		speed.Set(1, 0);
		speed.Normalize();
		speed = speed * 10;
		m_pUpperGunsWaves[ii]->SetSpawnSpeed(speed);
	}
}

void Ichi_Phase2_Attack1::UpdateMiddleGunWavesParameters()
{
	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase2GunsAttachPoints();
	const float SPEED = 10;

	Core::Mat44f meshWsTx = m_pP2Renderable->GetLocalTx().GetMatrix() * m_pIchi->GetTransform().GetWorldTx();

	Core::Vec4f zAxis = Core::Vec4f(0, 0, 1, 0) * SPEED;
	Core::Vec4f rotatedZAxis = zAxis * meshWsTx;
	Core::Vec4f negRotatedZAxis = rotatedZAxis * -1;

	Core::Vec4f xAxis = Core::Vec4f(1, 0, 0, 0) * SPEED;
	Core::Vec4f rotatedXAxis = xAxis * meshWsTx;
	Core::Vec4f negRotatedXAxis = rotatedXAxis * -1;

	for (uint8_t ii = 0; ii < m_waveCount; ++ii)
	{
		Core::Mat44f wsTx0 = pGunAttachPoints[ii * 2] * meshWsTx;
		Core::Mat44f wsTx1 = pGunAttachPoints[ii * 2 + 1] * meshWsTx;
		m_pWave[ii]->SetSpawnPosition(wsTx0.GetT(), wsTx1.GetT());

		switch (ii)
		{
		case 0:
			m_pWave[ii]->SetSpawnSpeed(rotatedZAxis);
			break;

		case 1:
			m_pWave[ii]->SetSpawnSpeed(negRotatedXAxis);
			break;

		case 2:
			m_pWave[ii]->SetSpawnSpeed(negRotatedZAxis);
			break;

		case 3:
			m_pWave[ii]->SetSpawnSpeed(rotatedXAxis);
			break;
		}
	}
}
