/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_BackBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_MainBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_SideBeam.h"

Ichi_Phase3_Attack2::Ichi_Phase3_Attack2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{
	const int BULLET_COUNT = 50;
	const int COUNTERABLE_BULLET_COUNT = BULLET_COUNT / 3;

	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
	{
		m_pMainBeam[ii] = new IchiWaveP1A2MainBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
		m_mainBeamIndex[ii] = UINT32_MAX;
	}

	for (uint32_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
	{
		m_pSideBeam[ii] = new IchiWaveP1A2SideBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), pIchi->GetCounterableBulletMaterial(), BULLET_COUNT, COUNTERABLE_BULLET_COUNT);
		m_sideBeamIndex[ii] = UINT32_MAX;
	}

	for (uint8_t ii = 0; ii < BACK_BEAM_COUNT; ++ii)
	{
		m_pBackBeam[ii] = new IchiWaveP1A2BackBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), pIchi->GetCounterableBulletMaterial(), BULLET_COUNT, COUNTERABLE_BULLET_COUNT);
		m_backBeamIndex[ii] = UINT32_MAX;
	}

	m_currentWaypointIndex = 0;
}

Ichi_Phase3_Attack2::~Ichi_Phase3_Attack2()
{
	DestroyWaves();

	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
	{
		delete m_pMainBeam[ii];
		m_pMainBeam[ii] = nullptr;
	}

	for (uint8_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
	{
		delete m_pSideBeam[ii];
		m_pSideBeam[ii] = nullptr;
	}

	for (uint8_t ii = 0; ii < BACK_BEAM_COUNT; ++ii)
	{
		delete m_pBackBeam[ii];
		m_pBackBeam[ii] = nullptr;
	}
}

void Ichi_Phase3_Attack2::OnEnter()
{
	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase3GunsAttachPoints();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
	{
		Core::Mat44f mainBeamWsTx = pGunAttachPoints[ii + 10] * m_pIchi->GetTransform().GetWorldTx();

		m_pMainBeam[ii]->SetSpawnPosition(mainBeamWsTx.GetT());
		m_pMainBeam[ii]->SetSpawnSpeed(Core::Vec4f(0, 0, -10, 0));
		pSubsystem->StartWave(m_mainBeamIndex[ii]);
	}

	//right side beam
	for(uint8_t ii = 0; ii < 5; ++ii)
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[ii + 12] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[ii]->SetSpawnPosition(sideBeamWsTx.GetT());
		m_pSideBeam[ii]->SetSpawnSpeed(Core::Vec4f(10, 0, 0, 0));
		pSubsystem->StartWave(m_sideBeamIndex[ii]);
	}

	//left side beam
	for (uint8_t ii = 0; ii < 5; ++ii)
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[ii + 5] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[ii + 5]->SetSpawnPosition(sideBeamWsTx.GetT());
		m_pSideBeam[ii + 5]->SetSpawnSpeed(Core::Vec4f(-10, 0, 0, 0));
		pSubsystem->StartWave(m_sideBeamIndex[ii + 5]);
	}

	//back beam
	for (uint8_t ii = 0; ii < BACK_BEAM_COUNT; ++ii)
	{
		Core::Mat44f backBeamWsTx = pGunAttachPoints[(17+ii) % 20] * m_pIchi->GetTransform().GetWorldTx();
		m_pBackBeam[ii]->SetSpawnPosition(backBeamWsTx.GetT());
		m_pBackBeam[ii]->SetSpawnSpeed(Core::Vec4f(0, 0, 10, 0));
		pSubsystem->StartWave(m_backBeamIndex[ii]);
	}

	m_waypoints[0] = m_pIchi->GetTransform().GetLocalTx().GetT() + Core::Vec4f(20, 0, 0, 0);
	m_waypoints[1] = m_pIchi->GetTransform().GetLocalTx().GetT() - Core::Vec4f(20, 0, 0, 0);

	m_currentWaypointIndex = 0;

	m_pIchi->GoToMotionStateTravel(m_waypoints[m_currentWaypointIndex]);
}

void Ichi_Phase3_Attack2::OnUpdate()
{
	if (m_pIchi->IsInMotionState(IchiMotionState::STOP))
	{
		++m_currentWaypointIndex;
		if (m_currentWaypointIndex >= 2)
		{
			GoTo(IchiStateEnum::PHASE3_TRAVEL);
			return;
		}

		m_pIchi->GoToMotionStateTravel(m_waypoints[m_currentWaypointIndex]);
	}

	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase3GunsAttachPoints();

	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
	{
		Core::Mat44f mainBeamWsTx = pGunAttachPoints[ii + 10] * m_pIchi->GetTransform().GetWorldTx();
		m_pMainBeam[ii]->SetSpawnPosition(mainBeamWsTx.GetT());
	}

	//left side beam
	for (uint8_t ii = 0; ii < 5; ++ii)
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[ii + 5] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[ii + 5]->SetSpawnPosition(sideBeamWsTx.GetT());
	}

	//right side beam
	for (uint8_t ii = 0; ii < 5; ++ii)
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[ii + 12] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[ii]->SetSpawnPosition(sideBeamWsTx.GetT());
	}

	//back beam
	for (uint8_t ii = 0; ii < BACK_BEAM_COUNT; ++ii)
	{
		Core::Mat44f backBeamWsTx = pGunAttachPoints[(17 + ii) % 20] * m_pIchi->GetTransform().GetWorldTx();
		m_pBackBeam[ii]->SetSpawnPosition(backBeamWsTx.GetT());
	}
}

void Ichi_Phase3_Attack2::OnExit()
{
	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
		m_pMainBeam[ii]->DisableSpawn();

	for (uint32_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
		m_pSideBeam[ii]->DisableSpawn();

	for (uint8_t ii = 0; ii < BACK_BEAM_COUNT; ++ii)
		m_pBackBeam[ii]->DisableSpawn();
}

void Ichi_Phase3_Attack2::InitWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	if (m_mainBeamIndex[0] == UINT32_MAX)
	{
		for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
		{
			m_mainBeamIndex[ii] = pSubsystem->AddWave(m_pMainBeam[ii]);
			pSubsystem->InitWave(m_mainBeamIndex[ii]);
		}
	}

	if (m_sideBeamIndex[0] == UINT32_MAX)
	{
		for (uint32_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
		{
			m_sideBeamIndex[ii] = pSubsystem->AddWave(m_pSideBeam[ii]);
			pSubsystem->InitWave(m_sideBeamIndex[ii]);
		}
	}

	if (m_backBeamIndex[0] == UINT32_MAX)
	{
		for (uint32_t ii = 0; ii < BACK_BEAM_COUNT; ++ii)
		{
			m_backBeamIndex[ii] = pSubsystem->AddWave(m_pBackBeam[ii]);
			pSubsystem->InitWave(m_backBeamIndex[ii]);
		}
	}
}

void Ichi_Phase3_Attack2::DestroyWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	if (m_mainBeamIndex[0] != UINT32_MAX)
	{
		for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
		{
			pSubsystem->DestroyWave(m_mainBeamIndex[ii]);
			pSubsystem->RemoveWave(m_mainBeamIndex[ii]);
			m_mainBeamIndex[ii] = UINT32_MAX;
		}
	}

	if (m_sideBeamIndex[0] != UINT32_MAX)
	{
		for (uint32_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
		{
			pSubsystem->DestroyWave(m_sideBeamIndex[ii]);
			pSubsystem->RemoveWave(m_sideBeamIndex[ii]);
			m_sideBeamIndex[ii] = UINT32_MAX;
		}
	}

	if (m_backBeamIndex[0] != UINT32_MAX)
	{
		for (uint32_t ii = 0; ii < BACK_BEAM_COUNT; ++ii)
		{
			pSubsystem->DestroyWave(m_backBeamIndex[ii]);
			pSubsystem->RemoveWave(m_backBeamIndex[ii]);
			m_backBeamIndex[ii] = UINT32_MAX;
		}
	}
}