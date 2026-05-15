/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/Waves/WaveMachineGun.h"

#include "Alpha/Bullets/Bullets.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Vec4f.h"
#include "Core/Random/Random.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderView.h"
#include "Systems/Objects/GameObject.h"

#include <assert.h>
#include <cmath>

WaveMachineGun::WaveMachineGun(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial, 
	const Systems::GameObject* pOwner, const Systems::GameObject* pTarget)
	: IBulletWave()
	, m_lastSpawnTime(0)
	, m_pOwner(pOwner)
	, m_pTarget(pTarget)
	, m_nextBulletToShot(0)
	, m_pCounterBulletMaterial(pCounterBulletMaterial)
{
	m_count = 20;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;
}

WaveMachineGun::~WaveMachineGun()
{
}

void WaveMachineGun::Init(Bullets& bullets)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;
	m_nextBulletToShot = m_startId;
}

void WaveMachineGun::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
}

void WaveMachineGun::Start(Bullets& bullets, const Core::Vec4f& /*pos*/)
{
	m_isAlive = true;
	m_lastSpawnTime = 0;
	m_nextBulletToShot = m_startId;

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		bullets.m_type[ii] = BulletType::NORMAL;
		bullets.m_state[ii] = BulletState::ATTACK;
	}

	//generate the counter bullets
	Core::RandomUInt generator(m_startId, m_endId - 1);

	const uint32_t COUNTER_BULLET_COUNT = 5;
	for (uint32_t ii = 0; ii < COUNTER_BULLET_COUNT; ++ii)
	{
		uint32_t index = generator.Generate();
		bullets.m_type[index] = BulletType::COUNTER;
	}
}

void WaveMachineGun::Stop()
{
	m_isAlive = false;
}

void WaveMachineGun::Update(Bullets& bullets, float dt)
{
	if (!m_isAlive)
		return;

	//reduce ttl
	for (uint32_t ii = m_startId; ii < m_nextBulletToShot; ++ii)
		bullets.m_timeToLive[ii] = bullets.m_timeToLive[ii] - dt;

	//compute new position
	for (uint32_t ii = m_startId; ii < m_nextBulletToShot; ++ii)
		bullets.m_positions[ii] = bullets.m_positions[ii] + bullets.m_speed[ii] * dt;

	//shoot a new bullet
	const float SHOOT_GAP_TIME = 0.5f;
	const Systems::IClockSubsystem* pClock = Systems::GameMgr::Get().GetWorld()->m_pClock;
	if (m_lastSpawnTime + SHOOT_GAP_TIME < pClock->GetTime() && m_nextBulletToShot < m_endId)
	{
		Core::Vec4f start = m_pOwner->GetTransform().GetWorldTx().GetT();
		Core::Vec4f end = m_pTarget->GetTransform().GetWorldTx().GetT();

		const float SPEED = 25.f;
		Core::Vec4f velocity = end - start;
		velocity.Normalize();
		velocity = velocity * SPEED;

		bullets.m_positions[m_nextBulletToShot] = start;
		bullets.m_speed[m_nextBulletToShot] = velocity;
		bullets.m_acceleration[m_nextBulletToShot] = Core::Vec4f(0, 0, 0, 0);
		bullets.m_timeToLive[m_nextBulletToShot] = 3;

		++m_nextBulletToShot;

		m_lastSpawnTime = pClock->GetTime();
	}
}

void WaveMachineGun::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
{
	if (!m_isAlive)
		return;

	m_isAlive = false;

	for (uint32_t ii = m_startId; ii < m_nextBulletToShot; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		Systems::RenderableObject& obj = scene.m_opaqueObjects.PushBackDefault();
		obj.m_pMesh = m_pMesh->GetRenderingMesh();

		if(bullets.m_type[ii] == BulletType::NORMAL)
			obj.m_pMaterial = m_pMaterial;
		else if(bullets.m_type[ii] == BulletType::COUNTER)
			obj.m_pMaterial = m_pCounterBulletMaterial;

		obj.m_pOwner = nullptr;
		obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
		obj.m_worldTx = Core::Mat44f::CreateTranslationMatrix(bullets.m_positions[ii]);

		if (bullets.m_type[ii] == BulletType::COUNTER)
		{
			Systems::RenderableObject& debugObj = scene.m_opaqueObjects.PushBackDefault();
			debugObj.DebugSphere(bullets.m_positions[ii], m_counterBulletCollisionRadius, Core::Float4(0, 1, 0, 1), true);
		}

		m_isAlive = true;
	}

	if (m_nextBulletToShot < m_endId)
		m_isAlive = true;
}
