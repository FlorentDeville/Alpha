/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_MainBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_SideBeam.h"

Ichi_Phase1_Attack2::Ichi_Phase1_Attack2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{
	const int BULLET_COUNT = 50;
	m_pMainBeam = new IchiWaveP1A2MainBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
	m_mainBeamIndex = UINT32_MAX;

	for (uint32_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
	{
		m_pSideBeam[ii] = new IchiWaveP1A2SideBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
		m_sideBeamIndex[ii] = UINT32_MAX;
	}
	
	m_currentWaypointIndex = 0;
}

Ichi_Phase1_Attack2::~Ichi_Phase1_Attack2()
{
	DestroyWaves();

	delete m_pMainBeam;
	m_pMainBeam = nullptr;

	for (uint8_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
	{
		delete m_pSideBeam[ii];
		m_pSideBeam[ii] = nullptr;
	}
}

void Ichi_Phase1_Attack2::OnEnter()
{
	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase1GunsAttachPoints();
	Core::Mat44f wsTx = pGunAttachPoints[0] * m_pIchi->GetTransform().GetWorldTx();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	m_pMainBeam->SetSpawnPosition(wsTx.GetT());
	m_pMainBeam->SetSpawnSpeed(Core::Vec4f(0, 0, -10, 0));
	pSubsystem->StartWave(m_mainBeamIndex);

	//left side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[3] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[0]->SetSpawnPosition(sideBeamWsTx.GetT());
		m_pSideBeam[0]->SetSpawnSpeed(Core::Vec4f(-10, 0, 0, 0));

		pSubsystem->StartWave(m_sideBeamIndex[0]);
	}

	//right side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[1] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[1]->SetSpawnPosition(sideBeamWsTx.GetT());
		m_pSideBeam[1]->SetSpawnSpeed(Core::Vec4f(10, 0, 0, 0));

		pSubsystem->StartWave(m_sideBeamIndex[1]);
	}

	m_waypoints[0] = m_pIchi->GetTransform().GetLocalTx().GetT() + Core::Vec4f(20, 0, 0, 0);
	m_waypoints[1] = m_pIchi->GetTransform().GetLocalTx().GetT() - Core::Vec4f(20, 0, 0, 0);

	m_currentWaypointIndex = 0;

	m_pIchi->GoToMotionStateTravel(m_waypoints[m_currentWaypointIndex]);
}

void Ichi_Phase1_Attack2::OnUpdate()
{
	if (m_pIchi->IsInMotionState(IchiMotionState::STOP))
	{
		++m_currentWaypointIndex;
		if (m_currentWaypointIndex >= 2)
		{
			GoTo(IchiStateEnum::PHASE1_TRAVEL);
			return;
		}

		m_pIchi->GoToMotionStateTravel(m_waypoints[m_currentWaypointIndex]);
	}

	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase1GunsAttachPoints();
	Core::Mat44f wsTx = pGunAttachPoints[0] * m_pIchi->GetTransform().GetWorldTx();
	m_pMainBeam->SetSpawnPosition(wsTx.GetT());

	//left side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[3] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[0]->SetSpawnPosition(sideBeamWsTx.GetT());
	}

	//right side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[1] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[1]->SetSpawnPosition(sideBeamWsTx.GetT());
	}
}

void Ichi_Phase1_Attack2::OnExit()
{
	m_pMainBeam->DisableSpawn();
	m_pSideBeam[0]->DisableSpawn();
	m_pSideBeam[1]->DisableSpawn();
}

void Ichi_Phase1_Attack2::InitWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	if (m_mainBeamIndex == UINT32_MAX)
	{
		m_mainBeamIndex = pSubsystem->AddWave(m_pMainBeam);
		pSubsystem->InitWave(m_mainBeamIndex);
	}

	if (m_sideBeamIndex[0] == UINT32_MAX)
	{
		m_sideBeamIndex[0] = pSubsystem->AddWave(m_pSideBeam[0]);
		pSubsystem->InitWave(m_sideBeamIndex[0]);
	}

	if (m_sideBeamIndex[1] == UINT32_MAX)
	{
		m_sideBeamIndex[1] = pSubsystem->AddWave(m_pSideBeam[1]);
		pSubsystem->InitWave(m_sideBeamIndex[1]);
	}
}

void Ichi_Phase1_Attack2::DestroyWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	if (m_mainBeamIndex != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_mainBeamIndex);
		pSubsystem->RemoveWave(m_mainBeamIndex);
	}

	if (m_sideBeamIndex[0] != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_sideBeamIndex[0]);
		pSubsystem->RemoveWave(m_sideBeamIndex[0]);
	}

	if (m_sideBeamIndex[1] != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_sideBeamIndex[1]);
		pSubsystem->RemoveWave(m_sideBeamIndex[1]);
	}
}