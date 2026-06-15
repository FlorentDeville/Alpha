/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Attack1.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A1.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P2_A1.h"


#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase3_Attack1::Ichi_Phase3_Attack1(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_startTime(0)
	, m_internalState(START_LOWER_TOWER_ODD_GUNS)
	, m_startInternalStateTime()
{
	m_lowerTowerWaveCount = m_pIchi->GetPhase3GunsAttachPointsCount();

	m_ppWaves = new IchiWaveP1A1*[m_lowerTowerWaveCount];
	m_pWaveIndex = new uint32_t[m_lowerTowerWaveCount];

	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
	{
		m_ppWaves[ii] = new IchiWaveP1A1(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), 50);
		m_pWaveIndex[ii] = UINT32_MAX;
	}

	m_middleTowerWaveCount = m_pIchi->GetPhase2GunsAttachPointsCount() / 2;
	m_ppMiddleTowerWaves = new IchiWaveP2A1*[m_middleTowerWaveCount];
	m_pMiddleTowerWaveIndex = new uint32_t[m_middleTowerWaveCount];

	for (uint32_t ii = 0; ii < m_middleTowerWaveCount; ++ii)
	{
		m_ppMiddleTowerWaves[ii] = new IchiWaveP2A1(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), 50);
		m_pMiddleTowerWaveIndex[ii] = UINT32_MAX;
	}

	m_pMiddleTowerRenderable = m_pIchi->GetPhase2Renderable();
}

Ichi_Phase3_Attack1::~Ichi_Phase3_Attack1()
{
	DestroyWaves();

	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
		delete m_ppWaves[ii];

	delete[] m_ppWaves;
	m_ppWaves = nullptr;

	delete[] m_pWaveIndex;
	m_pWaveIndex = nullptr;

	for (uint32_t ii = 0; ii < m_middleTowerWaveCount; ++ii)
		delete m_ppMiddleTowerWaves[ii];

	delete[] m_ppMiddleTowerWaves;
	m_ppMiddleTowerWaves = nullptr;

	delete[] m_pMiddleTowerWaveIndex;
	m_pMiddleTowerWaveIndex = nullptr;
}

void Ichi_Phase3_Attack1::OnEnter()
{
	m_internalState = START_LOWER_TOWER_ODD_GUNS;

	/*BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint32_t ii = 0; ii < m_waveCount; ++ii)
	{
		const Core::Mat44f& ap = m_pIchi->GetPhase3GunsAttachPoints()[ii];
		Core::Mat44f wsTx = ap * m_pIchi->GetTransform().GetWorldTx();
		m_ppWaves[ii]->SetSpawnPosition(wsTx.GetT());
		m_ppWaves[ii]->SetSpawnSpeed(ap.GetT());

		pSubsystem->StartWave(m_pWaveIndex[ii]);
	}*/

	m_startTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_startInternalStateTime = m_startTime;
}

void Ichi_Phase3_Attack1::OnUpdate()
{
	//const float DURATION = 15;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	switch (m_internalState)
	{
	case START_LOWER_TOWER_ODD_GUNS:
	{
		UpdateLowerTowerOddGuns();

		for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
		{
			if (ii % 2 == 0)
				pSubsystem->StopWave(m_pWaveIndex[ii]);
			else
				pSubsystem->StartWave(m_pWaveIndex[ii]);
		}

		m_internalState = LOWER_TOWER_ODD_GUNS;
		m_startInternalStateTime = currentTime;
	}
	break;

	case LOWER_TOWER_ODD_GUNS:
	{
		UpdateLowerTowerOddGuns();

		const float DURATION = 5;
		if (IsTimeElasped(m_startInternalStateTime, currentTime, DURATION))
		{
			m_internalState = START_LOWER_TOWER_EVEN_GUNS;
			m_startInternalStateTime = currentTime;
		}
	}
	break;

	case START_LOWER_TOWER_EVEN_GUNS:
	{
		UpdateLowerTowerEvenGuns();

		for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
		{
			if (ii % 2 == 1)
				pSubsystem->StopWave(m_pWaveIndex[ii]);
			else
				pSubsystem->StartWave(m_pWaveIndex[ii]);
		}

		m_internalState = LOWER_TOWER_EVEN_GUNS;
		m_startInternalStateTime = currentTime;
	}
	break;

	case LOWER_TOWER_EVEN_GUNS:
	{
		UpdateLowerTowerEvenGuns();

		const float DURATION = 5;
		if (IsTimeElasped(m_startInternalStateTime, currentTime, DURATION))
		{
			m_internalState = START_MIDDLE_TOWER;
			m_startInternalStateTime = currentTime;
		}
	}
	break;

	case START_MIDDLE_TOWER:
	{
		for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
		{
			pSubsystem->StopWave(m_pWaveIndex[ii]);
		}

		UpdateMiddleTowerGuns();

		for (uint32_t ii = 0; ii < m_middleTowerWaveCount; ++ii)
		{
			pSubsystem->StartWave(m_pMiddleTowerWaveIndex[ii]);
		}

		m_internalState = MIDDLE_TOWER;
		m_startInternalStateTime = currentTime;
	}
	break;

	case MIDDLE_TOWER:
	{
		UpdateMiddleTowerGuns();

		const float DURATION = 5;
		if (IsTimeElasped(m_startInternalStateTime, currentTime, DURATION))
		{
			for (uint32_t ii = 0; ii < m_middleTowerWaveCount; ++ii)
				pSubsystem->StopWave(m_pMiddleTowerWaveIndex[ii]);

			m_internalState = START_UPPER_TOWER;
			m_startInternalStateTime = currentTime;
		}
	}
	break;


	}
}

void Ichi_Phase3_Attack1::OnExit()
{
	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
	{
		m_ppWaves[ii]->Stop();
	}
}

void Ichi_Phase3_Attack1::InitWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
	{
		if (m_pWaveIndex[ii] != UINT32_MAX)
			continue;

		m_pWaveIndex[ii] = pSubsystem->AddWave(m_ppWaves[ii]);
		pSubsystem->InitWave(m_pWaveIndex[ii]);
	}

	for (uint32_t ii = 0; ii < m_middleTowerWaveCount; ++ii)
	{
		if (m_pMiddleTowerWaveIndex[ii] != UINT32_MAX)
			continue;

		m_pMiddleTowerWaveIndex[ii] = pSubsystem->AddWave(m_ppMiddleTowerWaves[ii]);
		pSubsystem->InitWave(m_pMiddleTowerWaveIndex[ii]);
	}
}

void Ichi_Phase3_Attack1::DestroyWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
	{
		if (m_pWaveIndex[ii] == UINT32_MAX)
			continue;

		pSubsystem->DestroyWave(m_pWaveIndex[ii]);
		pSubsystem->RemoveWave(m_pWaveIndex[ii]);
		m_pWaveIndex[ii] = UINT32_MAX;
	}

	for (uint32_t ii = 0; ii < m_middleTowerWaveCount; ++ii)
	{
		if (m_pMiddleTowerWaveIndex[ii] == UINT32_MAX)
			continue;

		pSubsystem->DestroyWave(m_pMiddleTowerWaveIndex[ii]);
		pSubsystem->RemoveWave(m_pMiddleTowerWaveIndex[ii]);
		m_pMiddleTowerWaveIndex[ii] = UINT32_MAX;
	}
}

bool Ichi_Phase3_Attack1::IsTimeElasped(float start, float current, float duration) const
{
	if (start + duration >= current)
		return false;

	return true;
}

void Ichi_Phase3_Attack1::UpdateLowerTowerOddGuns()
{
	const Core::Mat44f* pAp = m_pIchi->GetPhase3GunsAttachPoints();

	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
	{
		if (ii % 2 == 0)
			continue;

		Core::Mat44f wsTx = pAp[ii] * m_pIchi->GetTransform().GetWorldTx();

		m_ppWaves[ii]->SetSpawnPosition(wsTx.GetT());
		m_ppWaves[ii]->SetSpawnSpeed(pAp[ii].GetT());
	}
}

void Ichi_Phase3_Attack1::UpdateLowerTowerEvenGuns()
{
	const Core::Mat44f* pAp = m_pIchi->GetPhase3GunsAttachPoints();

	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
	{
		if (ii % 2 == 1)
			continue;

		Core::Mat44f wsTx = pAp[ii] * m_pIchi->GetTransform().GetWorldTx();

		m_ppWaves[ii]->SetSpawnPosition(wsTx.GetT());
		m_ppWaves[ii]->SetSpawnSpeed(pAp[ii].GetT());
	}
}

void Ichi_Phase3_Attack1::UpdateMiddleTowerGuns()
{
	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase2GunsAttachPoints();
	const float SPEED = 10;

	Core::Mat44f meshWsTx = m_pMiddleTowerRenderable->GetLocalTx().GetMatrix() * m_pIchi->GetTransform().GetWorldTx();

	Core::Vec4f zAxis = Core::Vec4f(0, 0, 1, 0) * SPEED;
	Core::Vec4f rotatedZAxis = zAxis * meshWsTx;
	Core::Vec4f negRotatedZAxis = rotatedZAxis * -1;

	Core::Vec4f xAxis = Core::Vec4f(1, 0, 0, 0) * SPEED;
	Core::Vec4f rotatedXAxis = xAxis * meshWsTx;
	Core::Vec4f negRotatedXAxis = rotatedXAxis * -1;

	for (uint8_t ii = 0; ii < m_middleTowerWaveCount; ++ii)
	{
		Core::Mat44f wsTx0 = pGunAttachPoints[ii * 2] * meshWsTx;
		Core::Mat44f wsTx1 = pGunAttachPoints[ii * 2 + 1] * meshWsTx;
		m_ppMiddleTowerWaves[ii]->SetSpawnPosition(wsTx0.GetT(), wsTx1.GetT());

		switch (ii)
		{
		case 0:
			m_ppMiddleTowerWaves[ii]->SetSpawnSpeed(rotatedZAxis);
			break;

		case 1:
			m_ppMiddleTowerWaves[ii]->SetSpawnSpeed(negRotatedXAxis);
			break;

		case 2:
			m_ppMiddleTowerWaves[ii]->SetSpawnSpeed(negRotatedZAxis);
			break;

		case 3:
			m_ppMiddleTowerWaves[ii]->SetSpawnSpeed(rotatedXAxis);
			break;
		}
	}
}