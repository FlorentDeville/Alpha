/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Waves/Ichi_Wave_P3_A2.h"

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

IchiWaveP3A2::IchiWaveP3A2(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount)
	: IBulletWave()
	, m_currentState(State::WARMUP)
	, m_currentScale(0)
	, m_nextBulletToSpawn(0)
	, m_enableSpawn(true)
	, m_spawnPositionCount(0)
	, m_pSpawnPositions(nullptr)
	, m_internalStateStartTime(0)
	, m_pSpeed(nullptr)
{
	m_count = bulletCount;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;
}

IchiWaveP3A2::~IchiWaveP3A2()
{
	delete[] m_pSpawnPositions;
	m_pSpawnPositions = nullptr;

	delete[] m_pSpeed;
	m_pSpeed = nullptr;
}

void IchiWaveP3A2::Init(Bullets& bullets)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;
}

void IchiWaveP3A2::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
}

void IchiWaveP3A2::Start(Bullets& bullets)
{
	m_enableSpawn = true;

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		bullets.m_timeToLive[ii] = -1;
	}

	m_nextBulletToSpawn = m_startId + m_spawnPositionCount; //the first n bullets are for the warmup
	m_isAlive = true;
	m_currentState = State::WARMUP;
	m_currentScale = 0;
	m_internalStateStartTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
}

void IchiWaveP3A2::Stop()
{
	m_enableSpawn = false;
	m_currentState = STOP;
	m_currentScale = 0;
}

void IchiWaveP3A2::Update(Bullets& bullets, float dt)
{
	if (!m_isAlive)
		return;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();

	switch (m_currentState)
	{
	case State::WARMUP:
	{
		const float WARMUP_TIME = 0.7f;
		if (m_internalStateStartTime + WARMUP_TIME <= currentTime)
		{
			m_currentState = State::FIRE;
			m_currentScale = 1;
			m_internalStateStartTime = currentTime;
			break;
		}

		m_currentScale = (currentTime - m_internalStateStartTime) / WARMUP_TIME;

		for (uint32_t ii = 0; ii < m_spawnPositionCount; ++ii)
			bullets.m_positions[m_startId + ii] = m_pSpawnPositions[ii];
	}
	break;

	case State::FIRE:
	{
		SpawnBullet(bullets);
		m_currentScale = 0;
		m_currentState = State::WAIT_FOR_NEXT_WAVE;
		m_internalStateStartTime = currentTime;
	}
	break;

	case State::WAIT_FOR_NEXT_WAVE:
	{
		const float WAIT_TIME = 0.f;
		if (m_internalStateStartTime + WAIT_TIME <= currentTime)
		{
			m_currentState = State::WARMUP;
			m_internalStateStartTime = currentTime;
		}
	}
	break;

	case STOP:
		break;

	}

	uint32_t firstSpawnIndex = m_startId + m_spawnPositionCount;

	//reduce ttl
	for (uint32_t ii = firstSpawnIndex; ii < m_endId; ++ii)
		bullets.m_timeToLive[ii] = bullets.m_timeToLive[ii] - dt;

	//compute new position
	for (uint32_t ii = firstSpawnIndex; ii < m_endId; ++ii)
		bullets.m_positions[ii] = bullets.m_positions[ii] + bullets.m_speed[ii] * dt;
}

void IchiWaveP3A2::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
{
	if (!m_isAlive)
		return;

	uint32_t firstSpawnIndex = m_startId + m_spawnPositionCount;

	if (m_currentScale != 0)
	{
		for (uint32_t ii = 0; ii < m_spawnPositionCount; ++ii)
		{
			Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(m_currentScale);

			Systems::RenderableObject& obj = scene.m_opaqueObjects.PushBackDefault();
			obj.m_pMesh = m_pMesh->GetRenderingMesh();
			obj.m_pMaterial = m_pMaterial;
			obj.m_pOwner = nullptr;
			obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
			obj.m_worldTx = scale * Core::Mat44f::CreateTranslationMatrix(m_pSpawnPositions[ii]);
		}
	}

	for (uint32_t ii = firstSpawnIndex; ii < m_endId; ++ii)
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

void IchiWaveP3A2::SetSpawnPositionsCount(uint8_t count)
{
	m_spawnPositionCount = count;

	delete[] m_pSpawnPositions;
	delete[] m_pSpeed;

	if (count != 0)
	{
		m_pSpawnPositions = new Core::Vec4f[count];
		m_pSpeed = new Core::Vec4f[count];
	}
	else
	{
		m_pSpawnPositions = nullptr;
		m_pSpeed = nullptr;
	}
}

void IchiWaveP3A2::SetSpawnPosition(uint8_t index, const Core::Vec4f& position)
{
	assert(index < m_spawnPositionCount && "Invalid spawn index");

	m_pSpawnPositions[index] = position;
}

void IchiWaveP3A2::SetSpawnSpeed(uint8_t index, Core::Vec4f& speed)
{
	m_pSpeed[index] = speed;
}

void IchiWaveP3A2::SpawnBullet(Bullets& bullets)
{
	//assert(m_nextBulletToSpawn < m_endId);
	if (!m_enableSpawn)
		return;

	for (uint32_t ii = 0; ii < m_spawnPositionCount; ++ii)
	{
		bullets.m_timeToLive[m_nextBulletToSpawn] = 10;
		bullets.m_positions[m_nextBulletToSpawn] = m_pSpawnPositions[ii];
		bullets.m_speed[m_nextBulletToSpawn] = m_pSpeed[ii];
		bullets.m_acceleration[m_nextBulletToSpawn] = Core::Vec4f(0, 0, 0, 0);
		bullets.m_type[m_nextBulletToSpawn] = BulletType::NORMAL;

		++m_nextBulletToSpawn;
		if (m_nextBulletToSpawn >= m_endId)
			m_nextBulletToSpawn = m_startId + m_spawnPositionCount;
	}
}
