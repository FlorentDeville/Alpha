/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossStateWaveTest.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Bullets/Waves/WaveMachineGun.h"
#include "Alpha/Bullets/Waves/WaveTest.h"
#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/BossGameObject.h"

#include "Core/Log/LogModule.h"

#include "Systems/Objects/GameObject.h"

BossStateWaveTest::BossStateWaveTest(StateMachine* pStateMachine)
	: IState(pStateMachine)
	, m_pWave(nullptr)
	, m_pWaveMachineGun(nullptr)
	, m_pBoss(nullptr)
	, m_waveIndex(0)
	, m_waveMachineGunIndex(0)
	, m_runFirstWave(true)
{ }

BossStateWaveTest::~BossStateWaveTest()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->RemoveWave(m_waveIndex);
	pSubsystem->RemoveWave(m_waveMachineGunIndex);

	delete m_pWave;
	delete m_pWaveMachineGun;
}

void BossStateWaveTest::Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial,
	BossGameObject* pBoss, const Systems::GameObject* pTarget)
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	m_pBoss = pBoss;

	m_pWave = new WaveTest(pMesh, pMaterial);
	m_waveIndex = pSubsystem->AddWave(m_pWave);
	pSubsystem->InitWave(m_waveIndex);

	m_pWaveMachineGun = new WaveMachineGun(pMesh, pMaterial, pCounterBulletMaterial, pBoss, pTarget);
	m_waveMachineGunIndex = pSubsystem->AddWave(m_pWaveMachineGun);
	pSubsystem->InitWave(m_waveMachineGunIndex);
}

void BossStateWaveTest::OnEnter()
{
	const Systems::TransformComponent& transform = m_pBoss->GetTransform();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	if(m_runFirstWave)
		pSubsystem->StartWave(m_waveIndex, transform.GetWorldTx().GetT());
	else
		pSubsystem->StartWave(m_waveMachineGunIndex, transform.GetWorldTx().GetT());

	m_runFirstWave = !m_runFirstWave;
}

void BossStateWaveTest::OnUpdate()
{
	if (!m_pWave->IsAlive() && !m_pWaveMachineGun->IsAlive())
	{
		Core::LogModule::Get().LogInfo("BossStateWaveTest goto state WAIT");
		GoTo(BossStateEnum::WAIT);
	}
}

void BossStateWaveTest::OnExit()
{ }
