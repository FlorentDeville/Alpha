/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_BackBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_MainBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_SideBeam.h"

#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase1_Attack2::Ichi_Phase1_Attack2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{
	const int BULLET_COUNT = 50;
	const int COUNTERABLE_BULLET_COUNT = BULLET_COUNT / 3;

	m_pMainBeam = new IchiWaveP1A2MainBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), BULLET_COUNT);
	m_mainBeamIndex = UINT32_MAX;

	for (uint32_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
	{
		m_pSideBeam[ii] = new IchiWaveP1A2SideBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), pIchi->GetCounterableBulletMaterial(), BULLET_COUNT, COUNTERABLE_BULLET_COUNT);
		m_sideBeamIndex[ii] = UINT32_MAX;
	}

	m_pBackBeam = new IchiWaveP1A2BackBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), pIchi->GetCounterableBulletMaterial(), BULLET_COUNT, COUNTERABLE_BULLET_COUNT);
	m_backBeamIndex = UINT32_MAX;
	
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

	delete m_pBackBeam;
	m_pBackBeam = nullptr;
}

void Ichi_Phase1_Attack2::OnEnter()
{
	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase1GunsAttachPoints();
	Core::Mat44f wsTx = pGunAttachPoints[0] * m_pIchi->GetTransform().GetWorldTx();

	m_pMainBeam->SetSpawnSpeed(Core::Vec4f(0, 0, -10, 0));

	//left side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[3] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[0]->SetSpawnSpeed(Core::Vec4f(-10, 0, 0, 0));
	}

	//right side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[1] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[1]->SetSpawnSpeed(Core::Vec4f(10, 0, 0, 0));
	}

	//back beam
	{
		Core::Mat44f backBeamWsTx = pGunAttachPoints[2] * m_pIchi->GetTransform().GetWorldTx();
		m_pBackBeam->SetSpawnSpeed(Core::Vec4f(0, 0, 10, 0));
	}

	m_waypoints[0] = m_pIchi->GetTransform().GetLocalTx().GetT() + Core::Vec4f(20, 0, 0, 0);
	m_waypoints[1] = m_pIchi->GetTransform().GetLocalTx().GetT() - Core::Vec4f(20, 0, 0, 0);

	m_currentWaypointIndex = 0;
	m_internalState = InternalState::GET_IN_POSITION;
}

void Ichi_Phase1_Attack2::OnUpdate()
{
	const float STRAFE_SPEED = 2;

	switch (m_internalState)
	{
	case InternalState::GET_IN_POSITION:
	{
		float angleInDeg = 45;
		float angle = Core::PI_OVER_180 * angleInDeg;
		float halfAngle = angle * 0.5f;
		Core::Quaternion rot(0, sinf(halfAngle), 0, cosf(halfAngle));
		m_pIchi->GetTransform().SetLocalRotation(rot);

		m_internalState = InternalState::WAVE_WARMUP;
		m_startTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

		UpdateWaves();

		BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
		pSubsystem->StartWave(m_mainBeamIndex);
		pSubsystem->StartWave(m_sideBeamIndex[0]);
		pSubsystem->StartWave(m_sideBeamIndex[1]);
		pSubsystem->StartWave(m_backBeamIndex);
	}
	break;

	case InternalState::WAVE_WARMUP:
	{
		const float WARMUP_DURATION = 1.5f;
		
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		if (m_startTime + WARMUP_DURATION < currentTime)
		{
			m_internalState = InternalState::STRAFE;
			m_pIchi->GoToMotionStateStrafe(m_waypoints[m_currentWaypointIndex], STRAFE_SPEED);
		}
	}
	break;

	case InternalState::STRAFE:
	{
		if (m_pIchi->IsInMotionState(IchiMotionState::STOP))
		{
			++m_currentWaypointIndex;
			if (m_currentWaypointIndex >= 2)
			{
				m_pMainBeam->DisableSpawn();
				m_pSideBeam[0]->DisableSpawn();
				m_pSideBeam[1]->DisableSpawn();
				m_pBackBeam->DisableSpawn();

				m_pIchi->GoToMotionState(IchiMotionState::IDLE);

				m_restStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

				m_internalState = InternalState::REST;
				return;
			}

			m_pIchi->GoToMotionStateStrafe(m_waypoints[m_currentWaypointIndex], STRAFE_SPEED);
		}

		UpdateWaves();
	}
	break;

	case InternalState::REST:
	{
		const float REST_DURATION = 1;
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		if (m_restStartTime + REST_DURATION <= currentTime)
		{
			GoTo(IchiStateEnum::PHASE1_TRAVEL);
		}
	}
	break;

	}
}

void Ichi_Phase1_Attack2::OnExit()
{
	m_pMainBeam->DisableSpawn();
	m_pSideBeam[0]->DisableSpawn();
	m_pSideBeam[1]->DisableSpawn();
	m_pBackBeam->DisableSpawn();
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

	if (m_backBeamIndex == UINT32_MAX)
	{
		m_backBeamIndex = pSubsystem->AddWave(m_pBackBeam);
		pSubsystem->InitWave(m_backBeamIndex);
	}
}

void Ichi_Phase1_Attack2::DestroyWaves()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	if (m_mainBeamIndex != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_mainBeamIndex);
		pSubsystem->RemoveWave(m_mainBeamIndex);
		m_mainBeamIndex = UINT32_MAX;
	}

	if (m_sideBeamIndex[0] != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_sideBeamIndex[0]);
		pSubsystem->RemoveWave(m_sideBeamIndex[0]);
		m_sideBeamIndex[0] = UINT32_MAX;
	}

	if (m_sideBeamIndex[1] != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_sideBeamIndex[1]);
		pSubsystem->RemoveWave(m_sideBeamIndex[1]);
		m_sideBeamIndex[1] = UINT32_MAX;
	}

	if (m_backBeamIndex != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_backBeamIndex);
		pSubsystem->RemoveWave(m_backBeamIndex);
		m_backBeamIndex = UINT32_MAX;
	}
}

void Ichi_Phase1_Attack2::UpdateWaves()
{
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

	//back side beam
	{
		Core::Mat44f backBeamWsTx = pGunAttachPoints[2] * m_pIchi->GetTransform().GetWorldTx();
		m_pBackBeam->SetSpawnPosition(backBeamWsTx.GetT());
	}
}
