/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossStateWaveTest.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Bullets/Waves/WaveTest.h"
#include "Alpha/Components/Boss/BossComponent.h"
#include "Alpha/Components/Boss/States/BossStateEnum.h"

#include "Core/Log/LogModule.h"

#include "Systems/Objects/GameObject.h"

BossStateWaveTest::BossStateWaveTest(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_pWave(nullptr)
	, m_pBossComponent(nullptr)
	, m_waveIndex(0)
{ }

BossStateWaveTest::~BossStateWaveTest()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->RemoveWave(m_waveIndex);
	delete m_pWave;
}

void BossStateWaveTest::Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, BossComponent* pComponent)
{
	m_pBossComponent = pComponent;

	m_pWave = new WaveTest(pMesh, pMaterial);
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	m_waveIndex = pSubsystem->AddWave(m_pWave);
	pSubsystem->InitWave(m_waveIndex);
}

void BossStateWaveTest::OnEnter()
{
	const Systems::TransformComponent& transform = m_pBossComponent->GetOwner()->GetTransform();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->StartWave(m_waveIndex, transform.GetWorldTx().GetT());
}

void BossStateWaveTest::OnUpdate()
{
	if (!m_pWave->IsAlive())
	{
		Core::LogModule::Get().LogInfo("BossStateWaveTest goto state WAIT");
		GoTo(BossStateEnum::WAIT);
	}
}

void BossStateWaveTest::OnExit()
{ }
