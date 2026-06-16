/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_BackBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_MainBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_SideBeam.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase3_Attack2::Ichi_Phase3_Attack2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_internalState()
{
	const int BULLET_COUNT = 50;
	const int COUNTERABLE_BULLET_COUNT = BULLET_COUNT / 3;

	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
	{
		m_pMainBeam[ii] = new IchiWaveP1A2MainBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
		m_mainBeamIndex[ii] = UINT32_MAX;
	}

	m_pBackBeam = new IchiWaveP1A2BackBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), pIchi->GetCounterableBulletMaterial(), BULLET_COUNT, COUNTERABLE_BULLET_COUNT);
	m_backBeamIndex = UINT32_MAX;

	m_waypoints[0] = Core::Vec4f(-22, 0, 15, 1);
	m_waypoints[1] = Core::Vec4f(22, 0, 15, 1);
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

	delete m_pBackBeam;
	m_pBackBeam = nullptr;
}

void Ichi_Phase3_Attack2::OnEnter()
{
	const Core::Mat44f* pUpperTowerAttachPoints = m_pIchi->GetPhase1GunsAttachPoints();

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	//for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
	//{
	//	Core::Mat44f mainBeamWsTx = pGunAttachPoints[ii + 10] * m_pIchi->GetTransform().GetWorldTx();

	//	m_pMainBeam[ii]->SetSpawnPosition(mainBeamWsTx.GetT());
	//	m_pMainBeam[ii]->SetSpawnSpeed(Core::Vec4f(0, 0, -10, 0));
	//	pSubsystem->StartWave(m_mainBeamIndex[ii]);
	//}

	//m_currentWaypointIndex = 0;

	//m_pIchi->GoToMotionStateTravel(m_waypoints[m_currentWaypointIndex], 10);
	m_pIchi->GoToMotionState(IchiMotionState::IDLE);

	m_internalState = UPPER_TOWER_WAVES;
	m_internaStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	//back beam
	Core::Mat44f backBeamWsTx = pUpperTowerAttachPoints[2] * m_pIchi->GetTransform().GetWorldTx();
	m_pBackBeam->SetSpawnPosition(backBeamWsTx.GetT());
	m_pBackBeam->SetPreviousSpawnPosition(backBeamWsTx.GetT());
	m_pBackBeam->SetSpawnSpeed(Core::Vec4f(0, 0, 2, 0));
	pSubsystem->StartWave(m_backBeamIndex);
}

void Ichi_Phase3_Attack2::OnUpdate()
{
	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	switch (m_internalState)
	{
	case UPPER_TOWER_WAVES:
	{
		const float DURATION = 10;
		if (m_internaStateStartTime + DURATION <= currentTime)
		{
			m_pBackBeam->Stop();

			m_internalState = MIDDLE_TOWER_WAVES;
			m_internaStateStartTime = currentTime;

			//start middle tower waves
			const Core::Mat44f* pMiddleTowerAttachPoints = m_pIchi->GetPhase2GunsAttachPoints();
			for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
			{
				Core::Mat44f mainBeamWsTx = pMiddleTowerAttachPoints[ii] * m_pIchi->GetTransform().GetWorldTx();

				m_pMainBeam[ii]->SetSpawnPosition(mainBeamWsTx.GetT());
				m_pMainBeam[ii]->SetSpawnSpeed(Core::Vec4f(0, 0, -10, 0));
				pSubsystem->StartWave(m_mainBeamIndex[ii]);
			}
		}
	}
	break;

	case MIDDLE_TOWER_WAVES:
	{
		const float DURATION = 10;
		if (m_internaStateStartTime + DURATION <= currentTime)
		{
			for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
				m_pMainBeam[ii]->Stop();

			m_internalState = LOWER_TOWER_WAVES;
			m_internaStateStartTime = currentTime;
		}

	}
	break;

	case LOWER_TOWER_WAVES:
	{

	}
	break;
	}

	//if (m_pIchi->IsInMotionState(IchiMotionState::STOP))
	//{
	//	++m_currentWaypointIndex;
	//	if (m_currentWaypointIndex >= 2)
	//	{
	//		GoTo(IchiStateEnum::PHASE3_TRAVEL);
	//		return;
	//	}

	//	m_pIchi->GoToMotionStateTravel(m_waypoints[m_currentWaypointIndex], 10);
	//}

	//const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase3GunsAttachPoints();

	//for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
	//{
	//	Core::Mat44f mainBeamWsTx = pGunAttachPoints[ii + 10] * m_pIchi->GetTransform().GetWorldTx();
	//	m_pMainBeam[ii]->SetSpawnPosition(mainBeamWsTx.GetT());
	//}

	////back beam

	//Core::Mat44f backBeamWsTx = pGunAttachPoints[17 % 20] * m_pIchi->GetTransform().GetWorldTx();
	//m_pBackBeam->SetSpawnPosition(backBeamWsTx.GetT());
}

void Ichi_Phase3_Attack2::OnExit()
{
	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
		m_pMainBeam[ii]->Stop();

	m_pBackBeam->DisableSpawn();
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

	if (m_backBeamIndex == UINT32_MAX)
	{
		m_backBeamIndex = pSubsystem->AddWave(m_pBackBeam);
		pSubsystem->InitWave(m_backBeamIndex);
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

	if (m_backBeamIndex != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_backBeamIndex);
		pSubsystem->RemoveWave(m_backBeamIndex);
		m_backBeamIndex = UINT32_MAX;
	}
}