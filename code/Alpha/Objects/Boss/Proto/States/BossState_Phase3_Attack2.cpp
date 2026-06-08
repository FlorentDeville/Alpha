/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Proto/States/BossState_Phase3_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Proto/States/BossStateEnum.h"
#include "Alpha/Objects/Boss/Proto/BossGameObject.h"
#include "Alpha/Objects/Boss/Proto/Waves/WaveMachineGun.h"

#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Objects/GameObject.h"

BossState_Phase3_Attack2::BossState_Phase3_Attack2(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_pBoss(nullptr)
	, m_pTarget(nullptr)
	, m_pWave()
	, m_waveIndex()
	, m_lastWaveSpawnTime(0)
	, m_nextWaveToSpawn(0)
{
	m_waveIndex[0] = UINT32_MAX;
}

BossState_Phase3_Attack2::~BossState_Phase3_Attack2()
{
	DestroyWaves();

	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
		delete m_pWave[ii];
}

void BossState_Phase3_Attack2::Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial,
	BossGameObject* pBoss, const Systems::GameObject* pTarget)
{
	m_pBoss = pBoss;
	m_pTarget = pTarget;

	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		const uint32_t SIDE_BULLET_COUNT = 5;
		const uint32_t SINGLE_BULLET_LINE_COUNT = 3;
		const uint32_t TOTAL_BULLET_COUNT = SINGLE_BULLET_LINE_COUNT + (2 * SIDE_BULLET_COUNT * SINGLE_BULLET_LINE_COUNT);

		m_pWave[ii] = new WaveMachineGun(pMesh, pMaterial, pCounterBulletMaterial, pBoss, pTarget);
		m_pWave[ii]->SetSideBulletEnabled(true);
		m_pWave[ii]->SetSideBulletCount(SIDE_BULLET_COUNT);
		m_pWave[ii]->SetBulletCount(TOTAL_BULLET_COUNT);
		m_pWave[ii]->SetCounterableBulletCount(1);
		m_pWave[ii]->SetGapTime(0.05f);
		m_pWave[ii]->SetSpeed(45);
	}
}

void BossState_Phase3_Attack2::OnEnter()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->StartWave(m_waveIndex[0]);

	m_lastWaveSpawnTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_nextWaveToSpawn = 1;
}

void BossState_Phase3_Attack2::OnUpdate()
{
	const float DELAY = 0.75f;

	if (m_nextWaveToSpawn < WAVE_COUNT)
	{
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		if (currentTime >= m_lastWaveSpawnTime + DELAY)
		{
			BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
			pSubsystem->StartWave(m_waveIndex[m_nextWaveToSpawn]);

			++m_nextWaveToSpawn;
			m_lastWaveSpawnTime = currentTime;
		}
	}

	if (m_pBoss->GetCurrentHP() <= 0)
	{
		//winning state
		return;
	}

	bool allWaveDead = true;
	for(uint32_t ii = 0; ii < m_nextWaveToSpawn; ++ii)
	{
		if (m_pWave[ii]->IsAlive())
			allWaveDead = false;
	}

	if(m_nextWaveToSpawn >= WAVE_COUNT && allWaveDead)
		GoTo(BossStateEnum::PHASE3_TRAVEL);
}

void BossState_Phase3_Attack2::OnExit()
{
}

void BossState_Phase3_Attack2::InitWaves()
{
	if (m_waveIndex[0] != UINT32_MAX)
		return;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		m_waveIndex[ii] = pSubsystem->AddWave(m_pWave[ii]);
		pSubsystem->InitWave(m_waveIndex[ii]);
	}
}

void BossState_Phase3_Attack2::DestroyWaves()
{
	if (m_waveIndex[0] == UINT32_MAX)
		return;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		pSubsystem->DestroyWave(m_waveIndex[ii]);
		pSubsystem->RemoveWave(m_waveIndex[ii]);
	}

	m_waveIndex[0] = UINT32_MAX;
}
