/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Proto/States/BossState_Phase1_Attack.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Proto/States/BossStateEnum.h"
#include "Alpha/Objects/Boss/Proto/BossGameObject.h"
#include "Alpha/Objects/Boss/Proto/Waves/WaveMachineGun.h"
#include "Alpha/Objects/Boss/Proto/Waves/WaveTest.h"

#include "Core/Log/LogModule.h"

#include "Systems/Objects/GameObject.h"

BossState_Phase1_Attack::BossState_Phase1_Attack(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_pWave(nullptr)
	, m_pWaveMachineGun(nullptr)
	, m_pBoss(nullptr)
	, m_waveIndex(UINT32_MAX)
	, m_waveMachineGunIndex(UINT32_MAX)
	, m_runFirstWave(true)
{ }

BossState_Phase1_Attack::~BossState_Phase1_Attack()
{
	DestroyWaves();

	delete m_pWave;
	delete m_pWaveMachineGun;

	m_pWave = nullptr;
	m_pWaveMachineGun = nullptr;
}

void BossState_Phase1_Attack::Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial,
	BossGameObject* pBoss, const Systems::GameObject* pTarget)
{
	m_pBoss = pBoss;
	m_pWave = new WaveTest(pMesh, pMaterial);
	m_pWaveMachineGun = new WaveMachineGun(pMesh, pMaterial, pCounterBulletMaterial, pBoss, pTarget);	

	pBoss->EnterPhase1();
}

void BossState_Phase1_Attack::OnEnter()
{
	const Systems::TransformComponent& transform = m_pBoss->GetTransform();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	if (m_runFirstWave)
	{
		m_pWave->SetStartPosition(transform.GetWorldTx().GetT());
		pSubsystem->StartWave(m_waveIndex, transform.GetWorldTx().GetT());
	}
	else
		pSubsystem->StartWave(m_waveMachineGunIndex, transform.GetWorldTx().GetT());

	m_runFirstWave = !m_runFirstWave;
}

void BossState_Phase1_Attack::OnUpdate()
{
	if (m_pBoss->GetCurrentHP() <= 0)
	{
		BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
		pSubsystem->StopWave(m_waveIndex);
		pSubsystem->StopWave(m_waveMachineGunIndex);
		
		GoTo(BossStateEnum::PHASE1_TO_PHASE2);
		return;
	}

	if (!m_pWave->IsAlive() && !m_pWaveMachineGun->IsAlive())
	{
		GoTo(BossStateEnum::PHASE1_TRAVEL);
	}
}

void BossState_Phase1_Attack::OnExit()
{ }

void BossState_Phase1_Attack::InitWaves()
{
	if (m_waveIndex != UINT32_MAX)
		return;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	m_waveIndex = pSubsystem->AddWave(m_pWave);
	pSubsystem->InitWave(m_waveIndex);

	m_waveMachineGunIndex = pSubsystem->AddWave(m_pWaveMachineGun);
	pSubsystem->InitWave(m_waveMachineGunIndex);
}

void BossState_Phase1_Attack::DestroyWaves()
{
	if (m_waveIndex == UINT32_MAX)
		return;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->DestroyWave(m_waveIndex);
	pSubsystem->RemoveWave(m_waveIndex);
	pSubsystem->DestroyWave(m_waveMachineGunIndex);
	pSubsystem->RemoveWave(m_waveMachineGunIndex);

	m_waveIndex = UINT32_MAX;
	m_waveMachineGunIndex = UINT32_MAX;
}
