/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Proto/States/BossState_Phase2_Attack1.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Proto/States/BossStateEnum.h"
#include "Alpha/Objects/Boss/Proto/BossGameObject.h"
#include "Alpha/Objects/Boss/Proto/Waves/WaveTest.h"

#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Objects/GameObject.h"

BossState_Phase2_Attack1::BossState_Phase2_Attack1(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_pBoss(nullptr)
	, m_pTarget(nullptr)
	, m_pWave()
	, m_waveIndex()
	, m_delayBetweenWave(0.5f)
	, m_lastWaveStartTime(0)
	, m_nextWaveToStart(0)
{
	m_waveIndex[0] = UINT32_MAX;
}

BossState_Phase2_Attack1::~BossState_Phase2_Attack1()
{
	DestroyWaves();
	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		delete m_pWave[ii];
	}
}

void BossState_Phase2_Attack1::Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* /*pCounterBulletMaterial*/,
	BossGameObject* pBoss, const Systems::GameObject* pTarget)
{
	m_pBoss = pBoss;
	m_pTarget = pTarget;

	const float ROTATION_OFFSET_INC = Core::TWO_PI / 360 * 5;
	const uint32_t BULLET_COUNT = 37;
	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		float rotationOffset = ROTATION_OFFSET_INC * ii;
		m_pWave[ii] = new WaveTest(pMesh, pMaterial, BULLET_COUNT, rotationOffset);
	}
}

void BossState_Phase2_Attack1::OnEnter()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	m_pWave[0]->SetStartPosition(m_pBoss->GetTransform().GetWorldTx().GetT());
	pSubsystem->StartWave(m_waveIndex[0], m_pBoss->GetTransform().GetWorldTx().GetT());

	m_lastWaveStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_nextWaveToStart = 1;
}

void BossState_Phase2_Attack1::OnUpdate()
{ 
	if (m_nextWaveToStart < WAVE_COUNT)
	{
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		if (currentTime >= m_lastWaveStartTime + m_delayBetweenWave)
		{
			BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
			m_pWave[m_nextWaveToStart]->SetStartPosition(m_pBoss->GetTransform().GetWorldTx().GetT());
			pSubsystem->StartWave(m_waveIndex[m_nextWaveToStart], m_pBoss->GetTransform().GetWorldTx().GetT());

			++m_nextWaveToStart;
			m_lastWaveStartTime = currentTime;
			return;
		}
	}

	bool allWaveDead = true;
	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		if (m_pWave[ii]->IsAlive())
		{
			allWaveDead = false;
			return;
		}
	}

	//all waves are dead
	GoTo(BossStateEnum::PHASE2_TRAVEL);

}

void BossState_Phase2_Attack1::OnExit()
{ }

void BossState_Phase2_Attack1::InitWaves()
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

void BossState_Phase2_Attack1::DestroyWaves()
{
	if (m_waveIndex[0] == UINT32_MAX)
		return;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	for (uint32_t ii = 0; ii < WAVE_COUNT; ++ii)
	{
		pSubsystem->DestroyWave(m_waveIndex[ii]);
		pSubsystem->RemoveWave(m_waveIndex[ii]);
		m_waveIndex[ii] = UINT32_MAX;
	}
}
