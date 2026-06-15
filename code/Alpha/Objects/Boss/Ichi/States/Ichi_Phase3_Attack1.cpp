/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Attack1.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A1.h"


#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase3_Attack1::Ichi_Phase3_Attack1(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_startTime(0)
	, m_internalState(START_LOWER_TOWER_ODD_GUNS)
{
	m_lowerTowerWaveCount = m_pIchi->GetPhase3GunsAttachPointsCount();

	m_ppWaves = new IchiWaveP1A1*[m_lowerTowerWaveCount];
	m_pWaveIndex = new uint32_t[m_lowerTowerWaveCount];

	for (uint32_t ii = 0; ii < m_lowerTowerWaveCount; ++ii)
	{
		m_ppWaves[ii] = new IchiWaveP1A1(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), 50);
		m_pWaveIndex[ii] = UINT32_MAX;
	}
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