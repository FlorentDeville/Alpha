/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Attack2.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_BackBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_MainBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_SideBeam.h"
#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P3_A2.h"

#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase3_Attack2::Ichi_Phase3_Attack2(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_internalState()
	, m_internaStateStartTime()
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

	const Core::Mat44f* pLowerTowerAttachPoints = m_pIchi->GetPhase3GunsAttachPoints();
	uint8_t lowerTowerGunsCount = static_cast<uint8_t>(m_pIchi->GetPhase3GunsAttachPointsCount());
	m_pLowerWave = new IchiWaveP3A2(pIchi->GetBulletMesh(), pIchi->GetBulletMaterial(), 100);
	m_pLowerWave->SetSpawnPositionsCount(lowerTowerGunsCount);
	m_lowerWaveIndex = UINT32_MAX;

	for (uint8_t ii = 0; ii < lowerTowerGunsCount; ++ii)
	{
		Core::Mat44f ap = pLowerTowerAttachPoints[ii] * m_pIchi->GetTransform().GetWorldTx();
		m_pLowerWave->SetSpawnPosition(ii, ap.GetT());

		Core::Vec4f speed = pLowerTowerAttachPoints[ii].GetT();
		m_pLowerWave->SetSpawnSpeed(ii, speed);
	}

	m_waypoints[0] = Core::Vec4f(-22, 0, 15, 1);
	m_waypoints[1] = Core::Vec4f(22, 0, 15, 1);
	m_currentWaypointIndex = 0;

	m_pUpperTowerRenderable = m_pIchi->GetPhase1Renderable();
	m_pLowerTowerRenderable = m_pIchi->GetPhase3Renderable();
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

	delete m_pLowerWave;
	m_pLowerWave = nullptr;
}

void Ichi_Phase3_Attack2::OnEnter()
{
	m_pIchi->GoToMotionState(IchiMotionState::IDLE);

	//GoToInternalStateUpperTowerWaves();
	//GoToInternalStateLowerTowerWaves();

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_internalState = PREPARE_UPPER_TOWER_WAVES;
	m_internaStateStartTime = currentTime;
}

void Ichi_Phase3_Attack2::OnUpdate()
{
	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();

	switch (m_internalState)
	{
	case PREPARE_UPPER_TOWER_WAVES:
	{
		const float destAngleInDeg = 45;
		const float destAngle = Core::PI_OVER_180 * destAngleInDeg;
		const float halfAngle = destAngle * 0.5f;
		Core::Quaternion targetRotation(0, sinf(halfAngle), 0, cosf(halfAngle));

		const Core::Quaternion& currentRotation = m_pUpperTowerRenderable->GetLocalTx().GetRotationQuaternion();

		float angle = Core::Quaternion::Angle(currentRotation, targetRotation);
		if (angle > Core::PI) angle = angle - Core::TWO_PI;
		
		if (angle < 0.001f && angle > -0.001f)
		{
			m_pUpperTowerRenderable->SetLocalRotation(targetRotation);
			GoToInternalStateUpperTowerWaves();
		}
		else
		{
			const float SPEED = 0.2f;
			float maxStep = SPEED * dt;
			Core::Quaternion newRotation = Core::Quaternion::RotateTowards(currentRotation, targetRotation, maxStep);
			m_pUpperTowerRenderable->SetLocalRotation(newRotation);
		}
	}
	break;

	case UPPER_TOWER_WAVES:
	{
		const float DURATION = 10;
		if (m_internaStateStartTime + DURATION <= currentTime)
		{
			m_pBackBeam->Stop();

			m_internalState = UPPER_TOWER_WAVES_REST;
			m_internaStateStartTime = currentTime;
		}
	}
	break;

	case UPPER_TOWER_WAVES_REST:
	{
		const Core::Quaternion GOAL = Core::Quaternion::FromEulerAngles(0, 0, 0);
		const Core::Quaternion current = m_pUpperTowerRenderable->GetLocalTx().GetRotationQuaternion();

		//Angle return a value in the range [-2pi, 2pi] But I need it in the range [pi, -pi]
		float angle = Core::Quaternion::Angle(current, GOAL);
		if (angle > Core::PI)
			angle = angle - Core::TWO_PI;

		if (angle < 0.0001f)
		{
			m_pUpperTowerRenderable->SetLocalRotation(GOAL);
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

			break;
		}

		float speed = 0.5f;
		float max = speed * dt;
		Core::Quaternion newRotation = Core::Quaternion::RotateTowards(current, GOAL, max);
		m_pUpperTowerRenderable->SetLocalRotation(newRotation);
	}
	break;

	case MIDDLE_TOWER_WAVES:
	{
		const float DURATION = 10;
		if (m_internaStateStartTime + DURATION <= currentTime)
		{
			for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
				m_pMainBeam[ii]->Stop();

			GoToInternalStateLowerTowerWaves();
		}

	}
	break;

	case LOWER_TOWER_WAVES:
	{
		RotateLowerTower(18 * Core::PI_OVER_180);
		UpdateLowerTowerWaves();

		const float DURATION = 5;
		if (m_internaStateStartTime + DURATION <= currentTime)
		{
			m_pLowerWave->Stop();

			m_internalState = LOWER_TOWER_REST;
			m_internaStateStartTime = currentTime;
		}
	}
	break;

	case LOWER_TOWER_REST:
	{
		const Core::Quaternion GOAL = Core::Quaternion::FromEulerAngles(0, 0, 0);
		const Core::Quaternion current = m_pLowerTowerRenderable->GetLocalTx().GetRotationQuaternion();

		//Angle return a value in the range [-2pi, 2pi] But I need it in the range [pi, -pi]
		float angle = Core::Quaternion::Angle(current, GOAL);
		if (angle > Core::PI)
			angle = angle - Core::TWO_PI;

		if (angle < 0.0001f)
		{
			m_pLowerTowerRenderable->SetLocalRotation(GOAL);
			m_internalState = PREPARE_UPPER_TOWER_WAVES;
			m_internaStateStartTime = currentTime;
			break;
		}

		float speed = 0.5f;
		float max = speed * dt;
		Core::Quaternion newRotation = Core::Quaternion::RotateTowards(current, GOAL, max);
		m_pLowerTowerRenderable->SetLocalRotation(newRotation);
	}
	break;

	}
}

void Ichi_Phase3_Attack2::OnExit()
{
	for (uint8_t ii = 0; ii < MAIN_BEAM_COUNT; ++ii)
		m_pMainBeam[ii]->Stop();

	m_pBackBeam->DisableSpawn();

	m_pLowerWave->Stop();
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

	if (m_lowerWaveIndex == UINT32_MAX)
	{
		m_lowerWaveIndex = pSubsystem->AddWave(m_pLowerWave);
		pSubsystem->InitWave(m_lowerWaveIndex);
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

	if (m_lowerWaveIndex != UINT32_MAX)
	{
		pSubsystem->DestroyWave(m_lowerWaveIndex);
		pSubsystem->RemoveWave(m_lowerWaveIndex);
		m_lowerWaveIndex = UINT32_MAX;
	}
}

void Ichi_Phase3_Attack2::GoToInternalStateUpperTowerWaves()
{
	m_internalState = UPPER_TOWER_WAVES;
	m_internaStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	//back beam
	const Core::Mat44f* pUpperTowerAttachPoints = m_pIchi->GetPhase1GunsAttachPoints();
	Core::Mat44f backBeamWsTx = pUpperTowerAttachPoints[2] * m_pUpperTowerRenderable->GetLocalTx().GetMatrix() * m_pIchi->GetTransform().GetWorldTx();
	m_pBackBeam->SetSpawnPosition(backBeamWsTx.GetT());
	m_pBackBeam->SetPreviousSpawnPosition(backBeamWsTx.GetT());
	m_pBackBeam->SetSpawnSpeed(Core::Vec4f(0, 0, 2, 0));

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->StartWave(m_backBeamIndex);
}

void Ichi_Phase3_Attack2::GoToInternalStateLowerTowerWaves()
{
	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	m_internalState = LOWER_TOWER_WAVES;
	m_internaStateStartTime = currentTime;

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->StartWave(m_lowerWaveIndex);
}

void Ichi_Phase3_Attack2::UpdateLowerTowerWaves()
{
	uint32_t count = m_pIchi->GetPhase3GunsAttachPointsCount();
	const Core::Mat44f* pAp = m_pIchi->GetPhase3GunsAttachPoints();

	Systems::RenderableComponent* pRenderable = m_pIchi->GetPhase3Renderable();

	Core::Mat44f pMeshWsTx = pRenderable->GetLocalTx().GetMatrix() * m_pIchi->GetTransform().GetWorldTx();

	for (uint8_t ii = 0; ii < count; ++ii)
	{
		Core::Mat44f ap = pAp[ii] * pMeshWsTx;
		m_pLowerWave->SetSpawnPosition(ii, ap.GetT());

		Core::Vec4f speed = ap.GetT() - pMeshWsTx.GetT();
		m_pLowerWave->SetSpawnSpeed(ii, speed);
	}
}

void Ichi_Phase3_Attack2::RotateLowerTower(float speed)
{
	float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	Core::Quaternion rotation = Core::Quaternion::FromEulerAngles(0, speed * dt, 0);

	Systems::RenderableComponent* pRenderable = m_pIchi->GetPhase3Renderable();
	const Core::Quaternion& currentRotation = pRenderable->GetLocalTx().GetRotationQuaternion();

	Core::Quaternion newRotation = rotation * currentRotation;
	pRenderable->SetLocalRotation(newRotation);
}