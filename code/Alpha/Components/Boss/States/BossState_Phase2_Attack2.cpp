/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossState_Phase2_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Bullets/Waves/WaveMachineGun.h"
#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/BossGameObject.h"

#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Objects/GameObject.h"

BossState_Phase2_Attack2::BossState_Phase2_Attack2(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_pBoss(nullptr)
	, m_pTarget(nullptr)
	, m_pWave(nullptr)
	, m_waveIndex()
{ }

BossState_Phase2_Attack2::~BossState_Phase2_Attack2()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->DestroyWave(m_waveIndex);
	pSubsystem->RemoveWave(m_waveIndex);
	delete m_pWave;
}

void BossState_Phase2_Attack2::Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial,
	BossGameObject* pBoss, const Systems::GameObject* pTarget)
{
	m_pBoss = pBoss;
	m_pTarget = pTarget;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	m_pWave = new WaveMachineGun(pMesh, pMaterial, pCounterBulletMaterial, pBoss, pTarget);
	m_pWave->SetSideBulletEnabled(true);
	m_pWave->SetBulletCount(30);
	m_pWave->SetCounterableBulletCount(7);

	m_waveIndex = pSubsystem->AddWave(m_pWave);
	pSubsystem->InitWave(m_waveIndex);
}

void BossState_Phase2_Attack2::OnEnter()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->StartWave(m_waveIndex, m_pBoss->GetTransform().GetWorldTx().GetT());
}

void BossState_Phase2_Attack2::OnUpdate()
{
	if (m_pBoss->GetCurrentHP() <= 0)
	{
		GoTo(BossStateEnum::PHASE3_TRAVEL);
		m_pWave->Stop();

		m_pBoss->SetCurrentHP(m_pBoss->GetMaxHP());
		return;
	}

	if (!m_pWave->IsAlive())
		GoTo(BossStateEnum::PHASE2_TRAVEL);
}

void BossState_Phase2_Attack2::OnExit()
{
}
