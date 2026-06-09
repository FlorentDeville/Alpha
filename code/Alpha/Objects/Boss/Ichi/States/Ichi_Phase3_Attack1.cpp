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
{
	m_waveCount = m_pIchi->GetPhase3GunsAttachPointsCount();

	m_ppWaves = new IchiWaveP1A1*[m_waveCount];
	m_pWaveIndex = new uint32_t[m_waveCount];

	for (uint32_t ii = 0; ii < m_waveCount; ++ii)
	{
		m_ppWaves[ii] = new IchiWaveP1A1(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), 50);
		m_pWaveIndex[ii] = UINT32_MAX;
	}
}

Ichi_Phase3_Attack1::~Ichi_Phase3_Attack1()
{
	DestroyWaves();

	for (uint32_t ii = 0; ii < m_waveCount; ++ii)
		delete m_ppWaves[ii];

	delete[] m_ppWaves;
	m_ppWaves = nullptr;

	delete[] m_pWaveIndex;
	m_pWaveIndex = nullptr;
}

void Ichi_Phase3_Attack1::OnEnter()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint32_t ii = 0; ii < m_waveCount; ++ii)
	{
		const Core::Mat44f& ap = m_pIchi->GetPhase3GunsAttachPoints()[ii];
		Core::Mat44f wsTx = ap * m_pIchi->GetTransform().GetWorldTx();
		m_ppWaves[ii]->SetSpawnPosition(wsTx.GetT());
		m_ppWaves[ii]->SetSpawnSpeed(ap.GetT());

		pSubsystem->StartWave(m_pWaveIndex[ii]);
	}

	m_startTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
}

void Ichi_Phase3_Attack1::OnUpdate()
{
	const float DURATION = 5;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	if (m_startTime + DURATION < currentTime)
		GoTo(IchiStateEnum::PHASE3_TRAVEL);
}

void Ichi_Phase3_Attack1::OnExit()
{
	for (uint32_t ii = 0; ii < m_waveCount; ++ii)
	{
		m_ppWaves[ii]->DisableSpawn();
	}
}

void Ichi_Phase3_Attack1::InitWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint32_t ii = 0; ii < m_waveCount; ++ii)
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

	for (uint32_t ii = 0; ii < m_waveCount; ++ii)
	{
		if (m_pWaveIndex[ii] == UINT32_MAX)
			continue;

		pSubsystem->DestroyWave(m_pWaveIndex[ii]);
		pSubsystem->RemoveWave(m_pWaveIndex[ii]);
		m_pWaveIndex[ii] = UINT32_MAX;
	}
}
