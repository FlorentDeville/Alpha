/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/Waves/WaveMachineGun.h"

#include "Alpha/Bullets/Bullets.h"
#include "Alpha/Objects/BossGameObject.h"
#include "Alpha/Objects/PlayerGameObject.h"

#include "Core/Math/Constants.h"
#include "Core/Random/Random.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Collision/CollisionSubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessageSubsystem.h"
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
	, m_counterBulletStartId(0)
	, m_counterBulletEndId(0)
	, m_nextCounterBulletId(0)
	, m_counterableBulletCount(15)
	, m_sideBulletEnabled(false)
	, m_gapTime(0.5f)
{
	m_count = 20;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;

	m_pCounterBulletState = new CounterBulletStruct[m_counterableBulletCount];
}

WaveMachineGun::~WaveMachineGun()
{
	delete[] m_pCounterBulletState;
}

void WaveMachineGun::Init(Bullets& bullets)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;
	m_nextBulletToShot = m_startId;

	m_counterBulletStartId = bullets.Allocate(m_counterableBulletCount);
	assert(m_counterBulletStartId != UINT32_MAX);
	m_counterBulletEndId = m_counterBulletStartId + m_counterableBulletCount;
	m_nextCounterBulletId = m_counterBulletStartId;
}

void WaveMachineGun::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
	bullets.Free(m_counterBulletStartId, m_counterableBulletCount);
}

void WaveMachineGun::Start(Bullets& bullets, const Core::Vec4f& /*pos*/)
{
	m_isAlive = true;
	m_lastSpawnTime = 0;
	m_nextBulletToShot = m_startId;
	m_nextCounterBulletId = m_counterBulletStartId;

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		bullets.m_type[ii] = BulletType::NORMAL;
		bullets.m_state[ii] = BulletState::ATTACK;
	}

	for (uint32_t ii = m_counterBulletStartId; ii < m_counterBulletEndId; ++ii)
	{
		bullets.m_type[ii] = BulletType::COUNTERABLE;
	}

	//generate the counter bullets
	Core::RandomUInt generator(m_startId, m_endId - 1);

	for (uint32_t ii = 0; ii < m_counterableBulletCount; ++ii)
	{
		uint32_t index = generator.Generate();
		bullets.m_type[index] = BulletType::COUNTERABLE;
	}
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

	UpdateCounteredBullets(bullets, dt);

	//shoot a new bullet
	const Systems::IClockSubsystem* pClock = Systems::GameMgr::Get().GetWorld()->m_pClock;
	if (m_lastSpawnTime + m_gapTime < pClock->GetTime() && m_nextBulletToShot < m_endId)
	{
		Core::Vec4f start = m_pOwner->GetTransform().GetWorldTx().GetT();
		Core::Vec4f end = m_pTarget->GetTransform().GetWorldTx().GetT();

		const float SPEED = 25.f;
		Core::Vec4f dir = end - start;
		dir.Normalize();
		Core::Vec4f velocity = dir * SPEED;

		bullets.m_positions[m_nextBulletToShot] = start;
		bullets.m_speed[m_nextBulletToShot] = velocity;
		bullets.m_acceleration[m_nextBulletToShot] = Core::Vec4f(0, 0, 0, 0);
		bullets.m_timeToLive[m_nextBulletToShot] = 3;

		++m_nextBulletToShot;

		m_lastSpawnTime = pClock->GetTime();

		if (m_sideBulletEnabled)
		{
			//left bullet
			Core::Vec4f dirTangent(dir.GetZ(), dir.GetY(), -dir.GetX(), 0);
			const float START_OFFSET = 2;

			const float SPEED_OFFSET = 2;
			if(m_nextBulletToShot < m_endId)
			{
				Core::Vec4f sideBulletStart = start + dirTangent * START_OFFSET;

				bullets.m_positions[m_nextBulletToShot] = sideBulletStart;
				bullets.m_speed[m_nextBulletToShot] = velocity + dirTangent * SPEED_OFFSET;
				bullets.m_acceleration[m_nextBulletToShot] = Core::Vec4f(0, 0, 0, 0);
				bullets.m_timeToLive[m_nextBulletToShot] = 3;
				++m_nextBulletToShot;
			}

			//right bullet
			if (m_nextBulletToShot < m_endId)
			{
				Core::Vec4f sideBulletStart = start - dirTangent * START_OFFSET;

				bullets.m_positions[m_nextBulletToShot] = sideBulletStart;
				bullets.m_speed[m_nextBulletToShot] = velocity - dirTangent * SPEED_OFFSET;
				bullets.m_acceleration[m_nextBulletToShot] = Core::Vec4f(0, 0, 0, 0);
				bullets.m_timeToLive[m_nextBulletToShot] = 3;
				++m_nextBulletToShot;
			}
		}
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
		else if(bullets.m_type[ii] == BulletType::COUNTERABLE)
			obj.m_pMaterial = m_pCounterBulletMaterial;

		obj.m_pOwner = nullptr;
		obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
		obj.m_worldTx = Core::Mat44f::CreateTranslationMatrix(bullets.m_positions[ii]);

		/*if (bullets.m_type[ii] == BulletType::COUNTER)
		{
			Systems::RenderableObject& debugObj = scene.m_opaqueObjects.PushBackDefault();
			debugObj.DebugSphere(bullets.m_positions[ii], m_counterBulletCollisionRadius, Core::Float4(0, 1, 0, 1), true);
		}*/

		m_isAlive = true;
	}

	for (uint32_t ii = m_counterBulletStartId; ii < m_nextCounterBulletId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		Systems::RenderableObject& obj = scene.m_opaqueObjects.PushBackDefault();
		obj.m_pMesh = m_pMesh->GetRenderingMesh();
		obj.m_pMaterial = m_pCounterBulletMaterial;

		obj.m_pOwner = nullptr;
		obj.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap;
		obj.m_worldTx = Core::Mat44f::CreateTranslationMatrix(bullets.m_positions[ii]);

		m_isAlive = true;

		/*{
			CounterBulletStruct& state = m_pCounterBulletState[ii - m_counterBulletStartId];

			Systems::RenderableObject& debugObj = scene.m_opaqueObjects.PushBackDefault();
			debugObj.DebugSphere(state.m_bezierP0, 1, Core::Float4(1, 0, 0, 1), true);
			
			Systems::RenderableObject& debugObj2 = scene.m_opaqueObjects.PushBackDefault();
			debugObj2.DebugSphere(state.m_bezierP1, 1, Core::Float4(0, 1, 0, 1), true);
		}*/
	}

	if (m_nextBulletToShot < m_endId)
		m_isAlive = true;
}

void WaveMachineGun::CollisionDetection(Bullets& bullets)
{
	BaseClass::CollisionDetection(bullets);

	for (uint32_t ii = m_counterBulletStartId; ii < m_nextCounterBulletId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		Systems::CollisionSubsystem* pColSubsystem = Systems::CollisionSubsystem::GetSubsystem();
		Systems::ShapeSphere colShape(Core::Vec4f(), m_bulletCollisionRadius, nullptr);

		colShape.SetCenter(bullets.m_positions[ii]);

		const Systems::ICollisionShape* pOther = nullptr;
		bool res = pColSubsystem->CollisionDetection(&colShape, &pOther);
		if (!res)
			continue;

		Systems::GameObject* pGo = pOther->GetOwner();
		if (!pGo)
			continue;

		//send message to the boss
		BossGameObject* pBoss = pGo->Cast<BossGameObject>();
		if (!pBoss)
			continue;

		Systems::GameMessageSubsystem* pMessage = Systems::GameMessageSubsystem::GetSubsystem();
		Systems::GameMessage msg;
		msg.m_id = CONSTSID("bullet_counter_collision");
		pMessage->SendMessage(pGo, msg);

		//kill the bullet
		bullets.m_timeToLive[ii] = 0;
	}
}

void WaveMachineGun::SpawnCounterBullet(Bullets& bullets, uint32_t index)
{
	//kill the current bullet
	bullets.m_timeToLive[index] = 0;

	//spawn a new counter bullet
	bullets.m_timeToLive[m_nextCounterBulletId] = 1;
	
	Core::Vec4f startPosition = bullets.m_positions[index];

	const BossGameObject* pTarget = Systems::GameMgr::Get().FindGameObject<BossGameObject>();
	Core::Vec4f targetPosition = pTarget->GetTransform().GetWorldTx().GetT();

	uint32_t bezierIndex = m_nextCounterBulletId - m_counterBulletStartId;
	m_pCounterBulletState[bezierIndex].m_bezierP0 = startPosition;

	Core::RandomUInt random(0, 1);
	int32_t side = (random.Generate() * 2) - 1;

	Core::Vec4f startToTarget = targetPosition - startPosition;
	Core::Vec4f orthogonal(startToTarget.GetZ(), 0, -startToTarget.GetX(), 0);
	orthogonal = orthogonal * static_cast<float>(side);
	Core::Vec4f p1 = startPosition + (startToTarget * 0.5) + (orthogonal * 0.5);
	m_pCounterBulletState[bezierIndex].m_bezierP1 = p1;

	++m_nextCounterBulletId;
}

void WaveMachineGun::SetSideBulletEnabled(bool enabled)
{
	m_sideBulletEnabled = enabled;
}

void WaveMachineGun::SetBulletCount(uint32_t count)
{
	m_count = count;
}

void WaveMachineGun::SetCounterableBulletCount(uint32_t count)
{
	m_counterableBulletCount = count;
}

void WaveMachineGun::SetGapTime(float gapTime)
{
	m_gapTime = gapTime;
}

void WaveMachineGun::UpdateCounteredBullets(Bullets& bullets, float dt)
{
	const BossGameObject* pTarget = Systems::GameMgr::Get().FindGameObject<BossGameObject>();
	Core::Vec4f targetPosition = pTarget->GetTransform().GetWorldTx().GetT();

	for (uint32_t ii = m_counterBulletStartId; ii < m_nextCounterBulletId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		uint32_t bezierIndex = ii - m_counterBulletStartId;

		const float COUNTERED_BULLET_SPEED_MULT = 2;
		float t = (1 - bullets.m_timeToLive[ii]) * COUNTERED_BULLET_SPEED_MULT;

		if (t > 1)
		{
			bullets.m_timeToLive[ii] = 0;
			continue;
		}

		float oneMinusT = 1 - t;

		Core::Vec4f newPosition = (m_pCounterBulletState[bezierIndex].m_bezierP0 * oneMinusT * oneMinusT) +
			(m_pCounterBulletState[bezierIndex].m_bezierP1 * 2 * oneMinusT * t) +
			(targetPosition * t * t);

		bullets.m_positions[ii] = newPosition;
	}

	for (uint32_t ii = m_counterBulletStartId; ii < m_nextCounterBulletId; ++ii)
		bullets.m_timeToLive[ii] = bullets.m_timeToLive[ii] - dt;
}
