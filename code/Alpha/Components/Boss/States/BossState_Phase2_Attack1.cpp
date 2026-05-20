/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossState_Phase2_Attack1.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Bullets/Waves/WaveTest.h"
#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/BossGameObject.h"

#include "Systems/Objects/GameObject.h"

BossState_Phase2_Attack1::BossState_Phase2_Attack1(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_pBoss(nullptr)
	, m_pTarget(nullptr)
	, m_pWave(nullptr)
	, m_waveIndex(UINT32_MAX)
{ }

BossState_Phase2_Attack1::~BossState_Phase2_Attack1()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->RemoveWave(m_waveIndex);

	delete m_pWave;
}

void BossState_Phase2_Attack1::Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* /*pCounterBulletMaterial*/,
	BossGameObject* pBoss, const Systems::GameObject* pTarget)
{
	m_pBoss = pBoss;
	m_pTarget = pTarget;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	m_pWave = new WaveTest(pMesh, pMaterial);
	m_waveIndex = pSubsystem->AddWave(m_pWave);
	pSubsystem->InitWave(m_waveIndex);
}

void BossState_Phase2_Attack1::OnEnter()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->StartWave(m_waveIndex, m_pBoss->GetTransform().GetWorldTx().GetT());
}

void BossState_Phase2_Attack1::OnUpdate()
{ 
	if (!m_pWave->IsAlive())
	{
		GoTo(BossStateEnum::PHASE2_TRAVEL);
		return;
	}
}

void BossState_Phase2_Attack1::OnExit()
{ }
