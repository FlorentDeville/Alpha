/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P1_A1.h"

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

IchiWaveP1A1::IchiWaveP1A1(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount, float rotationOffset)
	: IBulletWave()
	, m_warmupDuration(1.5f)
	, m_warmupElapsedTime(0)
	, m_currentState(State::WARMUP)
	, m_currentScale(0)
	, m_rotationOffset(rotationOffset)
	, m_nextBulletToSpawn(0)
{
	m_count = bulletCount;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;
}

IchiWaveP1A1::~IchiWaveP1A1()
{
}

void IchiWaveP1A1::Init(Bullets& bullets)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;
}

void IchiWaveP1A1::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
}

void IchiWaveP1A1::Start(Bullets& bullets)
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

	bullets.m_timeToLive[m_nextBulletToSpawn] = 10;
	bullets.m_positions[m_nextBulletToSpawn] = m_spawnPosition;
	bullets.m_speed[m_nextBulletToSpawn] = m_spawnSpeed;
	bullets.m_acceleration[m_nextBulletToSpawn] = Core::Vec4f(0, 0, 0, 0);
	bullets.m_type[m_nextBulletToSpawn] = BulletType::NORMAL;
}

void IchiWaveP1A1::Stop()
{
	m_enableSpawn = false;
}

void IchiWaveP1A1::Update(Bullets& bullets, float dt)
{
	const float LIFETIME = 10;
	const float SPAWN_RATE = 10; //in bullet per second
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
		//reduce ttl
		for (uint32_t ii = m_startId; ii < m_endId; ++ii)
			bullets.m_timeToLive[ii] = bullets.m_timeToLive[ii] - dt;

		//compute new position
		for (uint32_t ii = m_startId; ii < m_endId; ++ii)
			bullets.m_positions[ii] = bullets.m_positions[ii] + bullets.m_speed[ii] * dt;

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

void IchiWaveP1A1::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
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
		obj.m_pMaterial = m_pMaterial;
		obj.m_pOwner = nullptr;
		obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
		obj.m_worldTx = scale * Core::Mat44f::CreateTranslationMatrix(bullets.m_positions[ii]);
	}
}

void IchiWaveP1A1::SetSpawnPosition(const Core::Vec4f& spawnPosition)
{
	m_spawnPosition = spawnPosition;
}

void IchiWaveP1A1::SetSpawnSpeed(const Core::Vec4f& spawnSpeed)
{
	m_spawnSpeed = spawnSpeed;
}

void IchiWaveP1A1::SpawnBullet(Bullets& bullets)
{
	//assert(m_nextBulletToSpawn < m_endId);
	if (!m_enableSpawn)
		return;

	bullets.m_timeToLive[m_nextBulletToSpawn] = 10;
	bullets.m_positions[m_nextBulletToSpawn] = m_spawnPosition;
	bullets.m_speed[m_nextBulletToSpawn] = m_spawnSpeed;
	bullets.m_acceleration[m_nextBulletToSpawn] = Core::Vec4f(0, 0, 0, 0);
	bullets.m_type[m_nextBulletToSpawn] = BulletType::NORMAL;

	++m_nextBulletToSpawn;
	if (m_nextBulletToSpawn >= m_endId)
		m_nextBulletToSpawn = m_startId;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_lastBulletSpawnedTime = currentTime;
}
