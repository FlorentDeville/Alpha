/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A2_SideBeam.h"

#include "Alpha/Bullets/Bullets.h"

#include "Core/Bezier/Bezier.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Vec4f.h"
#include "Core/Random/Random.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderView.h"

#include <assert.h>
#include <cmath>

IchiWaveP1A2SideBeam::IchiWaveP1A2SideBeam(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterableMaterial, 
	uint32_t bulletCount, uint32_t counterableBulletCount)
	: IBulletWave()
	, m_warmupDuration(1.5f)
	, m_warmupElapsedTime(0)
	, m_currentState(State::WARMUP)
	, m_currentScale(0)
	, m_nextBulletToSpawn(0)
	, m_enableSpawn(true)
	, m_lastBulletSpawnedTime(0)
	, m_curveSide(1)
{
	m_count = bulletCount;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;

	m_pCounterableMaterial = pCounterableMaterial;
	m_counterableBulletCount = counterableBulletCount;

	m_pBezier = new Core::QuadraticBezier[m_count];
}

IchiWaveP1A2SideBeam::~IchiWaveP1A2SideBeam()
{
	delete[] m_pBezier;
	m_pBezier = nullptr;
}

void IchiWaveP1A2SideBeam::Init(Bullets& bullets)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;
}

void IchiWaveP1A2SideBeam::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
}

void IchiWaveP1A2SideBeam::Start(Bullets& bullets)
{
	m_enableSpawn = true;

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		bullets.m_timeToLive[ii] = -1;
	}

	m_nextBulletToSpawn = m_startId;
	m_isAlive = true;
	m_currentState = State::WARMUP;
	m_warmupElapsedTime = 0;
	m_currentScale = 0;

	SpawnBullet(bullets);

	for(uint32_t ii = m_startId; ii < m_endId; ++ii)
		bullets.m_type[ii] = BulletType::COUNTERABLE;
}

void IchiWaveP1A2SideBeam::Update(Bullets& bullets, float dt)
{
	const float LIFETIME = 10;
	const float SPAWN_RATE = 0.5f; //in bullet per second
	const float ELAPSED_TIME_PER_BULLET = 1.f / SPAWN_RATE; //time between each bullet spawn

	if (!m_isAlive)
		return;

	switch (m_currentState)
	{
	case State::WARMUP:
	{
		m_warmupElapsedTime += dt;
		if (m_warmupElapsedTime >= m_warmupDuration)
		{
			bullets.m_timeToLive[m_nextBulletToSpawn] = LIFETIME;
			++m_nextBulletToSpawn;
			m_lastBulletSpawnedTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
			m_currentState = State::FIRE;
			break;
		}

		m_currentScale = m_warmupElapsedTime / m_warmupDuration;
	}
	break;

	case State::FIRE:
	{
		const float SPEED = 25;
		float ds = SPEED * Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();

		for (uint32_t ii = m_startId; ii < m_endId; ++ii)
		{
			if (bullets.m_timeToLive[ii] < 0)
				continue;

			uint32_t bezierIndex = ii - m_startId;

			//m_pBezier[bezierIndex].m_p2 = m_target;

			float curveParam = bullets.m_acceleration[ii].GetX();
			curveParam += ds / m_pBezier[bezierIndex].EvaluateFirstDerivative(curveParam).Length();

			if (curveParam >= 1)
				bullets.m_timeToLive[ii] = -1;

			bullets.m_positions[ii] = m_pBezier[bezierIndex].Evaluate(curveParam);
			
			bullets.m_acceleration[ii].Set(0, curveParam);
		}
		
		//check if I have to spawn a new bullet
		float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
		if (m_lastBulletSpawnedTime + ELAPSED_TIME_PER_BULLET <= currentTime)
		{
			SpawnBullet(bullets);
		}
	}
	break;

	}
}

void IchiWaveP1A2SideBeam::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
{
	if (!m_isAlive)
		return;

	Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(m_currentScale);

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		Systems::RenderableObject& obj = scene.m_opaqueObjects.PushBackDefault();
		obj.m_pMesh = m_pMesh->GetRenderingMesh();

		if (bullets.m_type[ii] == BulletType::NORMAL)
			obj.m_pMaterial = m_pMaterial;
		else
			obj.m_pMaterial = m_pCounterableMaterial;

		obj.m_pOwner = nullptr;
		obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
		obj.m_worldTx = scale * Core::Mat44f::CreateTranslationMatrix(bullets.m_positions[ii]);
	}
}

void IchiWaveP1A2SideBeam::SetSpawnPosition(const Core::Vec4f& spawnPosition)
{
	m_spawnPosition = spawnPosition;
}

void IchiWaveP1A2SideBeam::SetSpawnSpeed(const Core::Vec4f& spawnSpeed)
{
	m_spawnSpeed = spawnSpeed;
}

void IchiWaveP1A2SideBeam::SetTargetPosition(const Core::Vec4f& target)
{
	m_target = target;
}

void IchiWaveP1A2SideBeam::SetCurveSide(float curveSide)
{
	m_curveSide = curveSide;
}

void IchiWaveP1A2SideBeam::DisableSpawn()
{
	m_enableSpawn = false;
}

void IchiWaveP1A2SideBeam::SpawnBullet(Bullets& bullets)
{
	//assert(m_nextBulletToSpawn < m_endId);
	if (!m_enableSpawn)
		return;

	bullets.m_timeToLive[m_nextBulletToSpawn] = 10;
	bullets.m_positions[m_nextBulletToSpawn] = m_spawnPosition;
	bullets.m_speed[m_nextBulletToSpawn] = m_spawnSpeed;
	bullets.m_acceleration[m_nextBulletToSpawn] = Core::Vec4f(0, 0, 0, 0);

	uint32_t bezierIndex = m_nextBulletToSpawn - m_startId;
	m_pBezier[bezierIndex].m_p0 = m_spawnPosition;
	m_pBezier[bezierIndex].m_p2 = m_target;

	Core::Vec4f dir = m_spawnPosition - m_target;
	Core::Vec4f orthoDir(dir.GetZ(), dir.GetY(), -dir.GetX(), 0);

	const float ORTHO_DIR_LENGTH = 1;
	m_pBezier[bezierIndex].m_p1 = m_target + (dir * 0.5f) + (orthoDir * ORTHO_DIR_LENGTH * m_curveSide);

	++m_nextBulletToSpawn;
	if (m_nextBulletToSpawn >= m_endId)
		m_nextBulletToSpawn = m_startId;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_lastBulletSpawnedTime = currentTime;
}
