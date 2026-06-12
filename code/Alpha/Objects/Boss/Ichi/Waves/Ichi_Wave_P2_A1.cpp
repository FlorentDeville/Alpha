/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P2_A1.h"

#include "Alpha/Bullets/Bullets.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Vec4f.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderView.h"

#include <assert.h>
#include <cmath>

IchiWaveP2A1::IchiWaveP2A1(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount)
	: IBulletWave()
	, m_warmupElapsedTime(0)
	, m_currentState(State::FIRE)
	, m_currentScale(0)
	, m_nextBulletToSpawn(0)
	, m_spawnIndex(0)
	, m_enableSpawn(true)
	, m_lastBulletSpawnedTime(0)
{
	m_count = bulletCount;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;
}

IchiWaveP2A1::~IchiWaveP2A1()
{
}

void IchiWaveP2A1::Init(Bullets& bullets)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;
}

void IchiWaveP2A1::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
}

void IchiWaveP2A1::Start(Bullets& bullets)
{
	m_enableSpawn = true;

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		bullets.m_timeToLive[ii] = -1;
	}

	m_nextBulletToSpawn = m_startId + 1;
	m_isAlive = true;
	m_currentState = State::WAIT_FOR_WARMUP;
	m_warmupElapsedTime = 0;
	m_currentScale = 0;
	m_spawnIndex = 0;
	m_lastBulletSpawnedTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_waitForWarmupStartTime = m_lastBulletSpawnedTime;

	/*bullets.m_timeToLive[m_nextBulletToSpawn] = 10;
	bullets.m_positions[m_nextBulletToSpawn] = m_spawnPosition;
	bullets.m_speed[m_nextBulletToSpawn] = m_spawnSpeed;
	bullets.m_acceleration[m_nextBulletToSpawn] = Core::Vec4f(0, 0, 0, 0);
	bullets.m_type[m_nextBulletToSpawn] = BulletType::NORMAL;*/
}

void IchiWaveP2A1::Stop()
{
	m_enableSpawn = false;
	m_currentState = STOP;
	m_currentScale = 0;
}

void IchiWaveP2A1::Update(Bullets& bullets, float dt)
{
	const float LIFETIME = 10;
	const float SPAWN_RATE = 4; //in bullet per second
	const float ELAPSED_TIME_PER_BULLET = 1.f / SPAWN_RATE; //time between each bullet spawn

	const float WARMUP_DURATION = 0.2f;

	if (!m_isAlive)
		return;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	switch (m_currentState)
	{
	case State::WAIT_FOR_WARMUP:
	{
		float waitDuration = ELAPSED_TIME_PER_BULLET - WARMUP_DURATION;
		assert(waitDuration >= 0);

		if (currentTime >= m_waitForWarmupStartTime + waitDuration)
		{
			m_currentState = WARMUP;
			bullets.m_timeToLive[m_startId] = LIFETIME;
			bullets.m_positions[m_startId] = m_spawnPosition[m_spawnIndex];
			m_currentScale = 0;
			m_warmupElapsedTime = 0;
		}
		
	}
	break;

	case State::WARMUP:
	{
		m_warmupElapsedTime += dt;
		if (m_warmupElapsedTime >= WARMUP_DURATION)
		{
			m_currentState = State::FIRE;
		}

		bullets.m_positions[m_startId] = m_spawnPosition[m_spawnIndex];
		m_currentScale = m_warmupElapsedTime / WARMUP_DURATION;
	}
	break;

	case State::FIRE:
	{
		//check if I have to spawn a new bullet
		if (m_lastBulletSpawnedTime + ELAPSED_TIME_PER_BULLET <= currentTime)
		{
			SpawnBullet(bullets);
			m_currentState = WAIT_FOR_WARMUP;
			m_waitForWarmupStartTime = m_lastBulletSpawnedTime;
		}
	}
	break;

	case STOP:
		break;

	}

	//reduce ttl
	for (uint32_t ii = m_startId + 1; ii < m_endId; ++ii)
		bullets.m_timeToLive[ii] = bullets.m_timeToLive[ii] - dt;

	//compute new position
	for (uint32_t ii = m_startId + 1; ii < m_endId; ++ii)
		bullets.m_positions[ii] = bullets.m_positions[ii] + bullets.m_speed[ii] * dt;
}

void IchiWaveP2A1::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
{
	if (!m_isAlive)
		return;

	{
		Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(m_currentScale);

		Systems::RenderableObject& obj = scene.m_opaqueObjects.PushBackDefault();
		obj.m_pMesh = m_pMesh->GetRenderingMesh();
		obj.m_pMaterial = m_pMaterial;
		obj.m_pOwner = nullptr;
		obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
		obj.m_worldTx = scale * Core::Mat44f::CreateTranslationMatrix(bullets.m_positions[m_startId]);
	}

	for (uint32_t ii = m_startId + 1; ii < m_endId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		Systems::RenderableObject& obj = scene.m_opaqueObjects.PushBackDefault();
		obj.m_pMesh = m_pMesh->GetRenderingMesh();
		obj.m_pMaterial = m_pMaterial;
		obj.m_pOwner = nullptr;
		obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
		obj.m_worldTx = Core::Mat44f::CreateTranslationMatrix(bullets.m_positions[ii]);
	}
}

void IchiWaveP2A1::SetSpawnPosition(const Core::Vec4f& p1, const Core::Vec4f& p2)
{
	m_spawnPosition[0] = p1;
	m_spawnPosition[1] = p2;
}

void IchiWaveP2A1::SetSpawnSpeed(const Core::Vec4f& spawnSpeed)
{
	m_spawnSpeed = spawnSpeed;
}

void IchiWaveP2A1::SpawnBullet(Bullets& bullets)
{
	//assert(m_nextBulletToSpawn < m_endId);
	if (!m_enableSpawn)
		return;

	bullets.m_timeToLive[m_nextBulletToSpawn] = 10;
	bullets.m_positions[m_nextBulletToSpawn] = m_spawnPosition[m_spawnIndex];
	bullets.m_speed[m_nextBulletToSpawn] = m_spawnSpeed;
	bullets.m_acceleration[m_nextBulletToSpawn] = Core::Vec4f(0, 0, 0, 0);
	bullets.m_type[m_nextBulletToSpawn] = BulletType::NORMAL;

	++m_nextBulletToSpawn;
	if (m_nextBulletToSpawn >= m_endId)
		m_nextBulletToSpawn = m_startId;

	m_spawnIndex = 1 - m_spawnIndex;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_lastBulletSpawnedTime = currentTime;
}
