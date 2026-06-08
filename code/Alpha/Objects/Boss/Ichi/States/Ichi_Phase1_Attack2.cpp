/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_MainBeam.h"

Ichi_Phase1_Attack2::Ichi_Phase1_Attack2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{
	const int BULLET_COUNT = 50;
	m_pMainBeam = new IchiWaveP1A2MainBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
	m_mainBeamIndex = UINT32_MAX;
}

Ichi_Phase1_Attack2::~Ichi_Phase1_Attack2()
{
	DestroyWaves();

	delete m_pMainBeam;
	m_pMainBeam = nullptr;
}

void Ichi_Phase1_Attack2::OnEnter()
{ }

void Ichi_Phase1_Attack2::OnUpdate()
{ }

void Ichi_Phase1_Attack2::OnExit()
{ }

void Ichi_Phase1_Attack2::InitWaves()
{
	if (m_mainBeamIndex != UINT32_MAX)
		return;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	m_mainBeamIndex = pSubsystem->AddWave(m_pMainBeam);
	pSubsystem->InitWave(m_mainBeamIndex);
}

void Ichi_Phase1_Attack2::DestroyWaves()
{
	if (m_mainBeamIndex == UINT32_MAX)
		return;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->DestroyWave(m_mainBeamIndex);
	pSubsystem->RemoveWave(m_mainBeamIndex);
}