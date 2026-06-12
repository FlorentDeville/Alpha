/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_BackBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_MainBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_SideBeam.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"

#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase2_Attack2::Ichi_Phase2_Attack2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_getInPositionStartTime(0)
	, m_restStartTime(0)
	, m_startTime(0)
	, m_internalState(InternalState::GET_IN_POSITION)
	, m_backAndForthCount(1)
{
	const int BULLET_COUNT = 50;
	const int COUNTERABLE_BULLET_COUNT = BULLET_COUNT / 3;
	const uint32_t MAIN_BEAM_BULLET_COUNT = 100;

	m_pMainBeam = new IchiWaveP1A2MainBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), MAIN_BEAM_BULLET_COUNT);
	m_mainBeamIndex = UINT32_MAX;

	for (uint32_t ii = 0; ii < SIDE_BEAM_COUNT; ++ii)
	{
		m_pSideBeam[ii] = new IchiWaveP1A2SideBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), pIchi->GetCounterableBulletMaterial(), BULLET_COUNT, COUNTERABLE_BULLET_COUNT);
		m_sideBeamIndex[ii] = UINT32_MAX;
	}

	m_pSideBeam[0]->SetCurveSide(-1);
	m_pSideBeam[1]->SetCurveSide(1);

	m_pBackBeam = new IchiWaveP1A2BackBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), pIchi->GetCounterableBulletMaterial(), BULLET_COUNT, COUNTERABLE_BULLET_COUNT);
	m_backBeamIndex = UINT32_MAX;

	for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
	{
		m_pMiddleMainBeam[ii] = new IchiWaveP1A2MainBeam(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), MAIN_BEAM_BULLET_COUNT);
		m_middleMainBeamIndex[ii] = UINT32_MAX;
	}

	m_currentWaypointIndex = 0;

	const float destAngleInDeg = 45;
	const float destAngle = Core::PI_OVER_180 * destAngleInDeg;
	const float halfAngle = destAngle * 0.5f;
	m_shootingRot = Core::Quaternion(0, sinf(halfAngle), 0, cosf(halfAngle));

	m_waypoints[0] = Core::Vec4f(-18, 0, 15, 1);
	m_waypoints[1] = Core::Vec4f(18, 0, 15, 1);

	m_pP2Renderable = m_pIchi->GetPhase2Renderable();
}

Ichi_Phase2_Attack2::~Ichi_Phase2_Attack2()
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

	for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
	{
		delete m_pMiddleMainBeam[ii];
		m_pMiddleMainBeam[ii] = nullptr;
	}
}

void Ichi_Phase2_Attack2::OnEnter()
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

	for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
	{
		m_pMiddleMainBeam[ii]->SetSpawnSpeed(Core::Vec4f(0, 0, -10, 0));
	}

	m_startingRot = m_pIchi->GetTransform().GetLocalSqt().GetRotationQuaternion();
	m_getInPositionStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	m_backAndForthCount = 1;
	m_currentWaypointIndex = 0;
	m_internalState = InternalState::GET_IN_POSITION;
}

void Ichi_Phase2_Attack2::OnUpdate()
{
	const float STRAFE_SPEED = 2;

	switch (m_internalState)
	{
	case InternalState::GET_IN_POSITION:
	{
		const float DURATION = 2.f;
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		float paramT = (currentTime - m_getInPositionStartTime) / DURATION;

		Core::Quaternion slerp = Core::Quaternion::Slerp(m_startingRot, m_shootingRot, paramT);
		m_pIchi->GetTransform().SetLocalRotation(slerp);

		if (m_getInPositionStartTime + DURATION <= currentTime)
		{
			m_pIchi->GetTransform().SetLocalRotation(m_shootingRot);
			m_internalState = InternalState::WAVE_WARMUP;
			m_startTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

			UpdateWaves();

			BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
			//pSubsystem->StartWave(m_mainBeamIndex);
			pSubsystem->StartWave(m_sideBeamIndex[0]);
			pSubsystem->StartWave(m_sideBeamIndex[1]);
			pSubsystem->StartWave(m_backBeamIndex);

			//pSubsystem->StartWave(m_middleMainBeamIndex[1]);
			for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
				pSubsystem->StartWave(m_middleMainBeamIndex[ii]);
		}
	}
	break;

	case InternalState::WAVE_WARMUP:
	{
		const float WARMUP_DURATION = 1.5f;

		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		if (m_startTime + WARMUP_DURATION < currentTime)
		{
			//start with the closest side
			float distance0 = (m_waypoints[0] - m_pIchi->GetTransform().GetWorldPosition()).Length2();
			float distance1 = (m_waypoints[1] - m_pIchi->GetTransform().GetWorldPosition()).Length2();

			if (distance0 < distance1)
				m_currentWaypointIndex = 0;
			else
				m_currentWaypointIndex = 1;

			m_internalState = InternalState::STRAFE;
			m_pIchi->GoToMotionStateStrafe(m_waypoints[m_currentWaypointIndex], STRAFE_SPEED);
		}
	}
	break;

	case InternalState::STRAFE:
	{
		if (m_pIchi->IsInMotionState(IchiMotionState::STOP))
		{
			m_currentWaypointIndex = (m_currentWaypointIndex + 1) % WAYPOINT_COUNT;
			if (m_backAndForthCount <= 0)
			{
				m_pMainBeam->Stop();
				m_pSideBeam[0]->DisableSpawn();
				m_pSideBeam[1]->DisableSpawn();
				m_pBackBeam->DisableSpawn();

				for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
					m_pMiddleMainBeam[ii]->Stop();

				m_pIchi->GoToMotionState(IchiMotionState::IDLE);

				m_restStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

				m_internalState = InternalState::REST;
				return;
			}

			m_pIchi->GoToMotionStateStrafe(m_waypoints[m_currentWaypointIndex], STRAFE_SPEED);
			--m_backAndForthCount;
		}
		UpdateMotionMiddleTower();
		UpdateWaves();
	}
	break;

	case InternalState::REST:
	{
		const float REST_DURATION = 1;
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

		float paramT = (currentTime - m_restStartTime) / REST_DURATION;

		Core::Quaternion slerp = Core::Quaternion::Slerp(m_shootingRot, m_startingRot, paramT);
		m_pIchi->GetTransform().SetLocalRotation(slerp);

		if (m_restStartTime + REST_DURATION <= currentTime)
		{
			m_pIchi->GetTransform().SetLocalRotation(m_startingRot);
			GoTo(IchiStateEnum::PHASE1_TRAVEL);
		}
	}
	break;

	}
}

void Ichi_Phase2_Attack2::OnExit()
{
	m_pMainBeam->Stop();
	m_pSideBeam[0]->DisableSpawn();
	m_pSideBeam[1]->DisableSpawn();
	m_pBackBeam->DisableSpawn();
}

void Ichi_Phase2_Attack2::InitWaves()
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

	for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
	{
		if (m_middleMainBeamIndex[ii] != UINT32_MAX)
			continue;

		m_middleMainBeamIndex[ii] = pSubsystem->AddWave(m_pMiddleMainBeam[ii]);
		pSubsystem->InitWave(m_middleMainBeamIndex[ii]);
	}
}

void Ichi_Phase2_Attack2::DestroyWaves()
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

	for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
	{
		if (m_middleMainBeamIndex[ii] == UINT32_MAX)
			continue;

		pSubsystem->DestroyWave(m_middleMainBeamIndex[ii]);
		pSubsystem->RemoveWave(m_middleMainBeamIndex[ii]);
		m_middleMainBeamIndex[ii] = UINT32_MAX;
	}
}

void Ichi_Phase2_Attack2::UpdateWaves()
{
	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase1GunsAttachPoints();
	Core::Mat44f wsTx = pGunAttachPoints[0] * m_pIchi->GetTransform().GetWorldTx();
	m_pMainBeam->SetSpawnPosition(wsTx.GetT());

	const PlayerGameObject* pPlayer = Systems::GameMgr::Get().FindGameObject<PlayerGameObject>();
	Core::Vec4f playerPosition = pPlayer->GetTransform().GetWorldTx().GetT();

	//left side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[3] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[0]->SetSpawnPosition(sideBeamWsTx.GetT());
		m_pSideBeam[0]->SetTargetPosition(playerPosition);
	}

	//right side beam
	{
		Core::Mat44f sideBeamWsTx = pGunAttachPoints[1] * m_pIchi->GetTransform().GetWorldTx();
		m_pSideBeam[1]->SetSpawnPosition(sideBeamWsTx.GetT());
		m_pSideBeam[1]->SetTargetPosition(playerPosition);
	}

	//back side beam
	{
		Core::Mat44f backBeamWsTx = pGunAttachPoints[2] * m_pIchi->GetTransform().GetWorldTx();
		m_pBackBeam->SetSpawnPosition(backBeamWsTx.GetT());
	}

	UpdateMiddleTowerWaves();
}

void Ichi_Phase2_Attack2::UpdateMiddleTowerWaves()
{
	const float SPEED = 10;
	Core::Mat44f meshWsTx = m_pP2Renderable->GetLocalTx().GetMatrix() * m_pIchi->GetTransform().GetWorldTx();

	Core::Vec4f zAxis = Core::Vec4f(0, 0, 1, 0) * SPEED;
	Core::Vec4f rotatedZAxis = zAxis * meshWsTx;
	Core::Vec4f negRotatedZAxis = rotatedZAxis * -1;

	Core::Vec4f xAxis = Core::Vec4f(1, 0, 0, 0) * SPEED;
	Core::Vec4f rotatedXAxis = xAxis * meshWsTx;
	Core::Vec4f negRotatedXAxis = rotatedXAxis * -1;

	const Core::Mat44f* pGunAttachPoints = m_pIchi->GetPhase2GunsAttachPoints();
	for (uint8_t ii = 0; ii < MIDDLE_MAIN_BEAM_COUNT; ++ii)
	{
		Core::Mat44f wsTxAp = pGunAttachPoints[ii] * meshWsTx;
		m_pMiddleMainBeam[ii]->SetSpawnPosition(wsTxAp.GetT());

		switch (ii)
		{
		case 0:
		case 1:
			m_pMiddleMainBeam[ii]->SetSpawnSpeed(rotatedZAxis);
			break;

		case 2:
		case 3:
			m_pMiddleMainBeam[ii]->SetSpawnSpeed(negRotatedXAxis);
			break;

		case 4:
		case 5:
			m_pMiddleMainBeam[ii]->SetSpawnSpeed(negRotatedZAxis);
			break;

		case 6:
		case 7:
			m_pMiddleMainBeam[ii]->SetSpawnSpeed(rotatedXAxis);
			break;
		}
	}
}

void Ichi_Phase2_Attack2::UpdateMotionMiddleTower()
{
	const float ROTATION_SPEED = 0.5;
	float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	float newRotation = ROTATION_SPEED * dt;

	//rotate upper tower
	{
		Core::Quaternion quat = m_pP2Renderable->GetLocalTx().GetRotationQuaternion();
		Core::Quaternion rot = Core::Quaternion::FromEulerAngles(0, -newRotation, 0);

		Core::Quaternion finalRot = rot * quat;

		m_pP2Renderable->SetLocalRotation(finalRot);
	}
}
