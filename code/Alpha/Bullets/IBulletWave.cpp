/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/IBulletWave.h"

#include "Alpha/Bullets/Bullets.h"
#include "Alpha/Objects/BossGameObject.h"
#include "Alpha/Objects/PlayerGameObject.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Collision/CollisionSubsystem.h"
#include "Systems/Game/Subsystems/Collision/Shapes/ShapeSphere.h"
#include "Systems/Game/Subsystems/Message/GameMessageSubsystem.h"
#include "Systems/Objects/GameObject.h"

IBulletWave::IBulletWave()
	: m_startId(0)
	, m_endId(0)
	, m_count(0)
	, m_pMesh(nullptr)
	, m_pMaterial(nullptr)
	, m_isAlive(false)
	, m_counterBulletCollisionRadius(5)
	, m_bulletCollisionRadius(0.5f)
{ }

IBulletWave::~IBulletWave()
{ }

void IBulletWave::Stop()
{
	m_isAlive = false;
}

void IBulletWave::CollisionDetection(Bullets& bullets)
{
	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		if (bullets.m_state[ii] == BulletState::ATTACK)
		{
			bool collided = false;
			if (bullets.m_type[ii] == BulletType::COUNTERABLE)
			{
				if (CollisionTestForBulletCounter(bullets, ii))
					collided = CollisionTestForBullet(bullets, ii);

				if (collided)
					Core::LogModule::Get().LogInfo("Collision detected for counter bullet");
			}
			else
			{
				collided = CollisionTestForBullet(bullets, ii);
			}

			if (collided)
				return;
		}
		else if (bullets.m_state[ii] == BulletState::COUNTER)
		{
			Systems::CollisionSubsystem* pColSubsystem = Systems::CollisionSubsystem::GetSubsystem();
			Systems::ShapeSphere colShape(Core::Vec4f(), 0.25f, nullptr);

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
}

void IBulletWave::SpawnCounterBullet(Bullets& bullets, uint32_t index)
{
	Core::LogModule::Get().LogInfo("Bullet %d countered", index);

	const BossGameObject* pTarget = Systems::GameMgr::Get().FindGameObject<BossGameObject>();
	Core::Vec4f targetPosition = pTarget->GetTransform().GetWorldTx().GetT();

	//compute new acceleration
	bullets.m_acceleration[index] = Core::Vec4f(0, 0, 0, 0);

	//compute new velocity
	const float SPEED = 20;
	Core::Vec4f velocity = targetPosition - bullets.m_positions[index];
	velocity.Normalize();
	velocity = velocity * SPEED;
	bullets.m_speed[index] = velocity;
	bullets.m_state[index] = BulletState::COUNTER;
	bullets.m_timeToLive[index] = 0;
}

uint32_t IBulletWave::GetStartId() const
{
	return m_startId;
}

uint32_t IBulletWave::GetEndId() const
{
	return m_endId;
}

bool IBulletWave::IsAlive() const
{
	return m_isAlive;
}

bool IBulletWave::CollisionTestForBullet(const Bullets& bullets, uint32_t index)
{
	Systems::CollisionSubsystem* pColSubsystem = Systems::CollisionSubsystem::GetSubsystem();
	Systems::ShapeSphere colShape(Core::Vec4f(), m_bulletCollisionRadius, nullptr);

	colShape.SetCenter(bullets.m_positions[index]);

	const Systems::ICollisionShape* pOther = nullptr;
	bool res = pColSubsystem->CollisionDetection(&colShape, &pOther);
	if (!res)
		return false;

	Systems::GameObject* pGo = pOther->GetOwner();
	if (!pGo)
		return false;

	if (!pGo->IsA<PlayerGameObject>())
		return false;

	//kill bullet
	bullets.m_timeToLive[index] = 0;

	//send message to player
	Systems::GameMessageSubsystem* pMessage = Systems::GameMessageSubsystem::GetSubsystem();
	Systems::GameMessage msg;
	msg.m_id = CONSTSID("bullet_collision");
	pMessage->SendMessage(pGo, msg);

	Core::LogModule::Get().LogInfo("Bullet collided with player");

	return true;
}

bool IBulletWave::CollisionTestForBulletCounter(const Bullets& bullets, uint32_t index)
{
	Systems::CollisionSubsystem* pColSubsystem = Systems::CollisionSubsystem::GetSubsystem();
	Systems::ShapeSphere colShape(Core::Vec4f(), m_counterBulletCollisionRadius, nullptr);

	colShape.SetCenter(bullets.m_positions[index]);

	const Systems::ICollisionShape* pOther = nullptr;
	bool res = pColSubsystem->CollisionDetection(&colShape, &pOther);
	if (!res)
		return false;

	Systems::GameObject* pGo = pOther->GetOwner();
	if (!pGo)
		return false;

	if (!pGo->IsA<PlayerGameObject>())
		return false;

	//send message to player
	Systems::GameMessageSubsystem* pMessage = Systems::GameMessageSubsystem::GetSubsystem();
	Systems::GameMessage msg;
	msg.m_id = CONSTSID("counter_bullet_collision");
	msg.m_param = index;
	pMessage->SendMessage(pGo, msg);

	//Core::LogModule::Get().LogInfo("Counter bullet collided with player");

	return true;
}
