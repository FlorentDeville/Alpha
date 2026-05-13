/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/IBulletWave.h"

#include "Alpha/Bullets/Bullets.h"
#include "Alpha/Components/PlayerComponent.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/Subsystems/Collision/CollisionSubsystem.h"
#include "Systems/Game/Subsystems/Collision/Shapes/ShapeSphere.h"
#include "Systems/Objects/GameObject.h"

IBulletWave::IBulletWave()
	: m_startId(0)
	, m_endId(0)
	, m_count(0)
	, m_pMesh(nullptr)
	, m_pMaterial(nullptr)
	, m_isAlive(false)
{ }

IBulletWave::~IBulletWave()
{ }

void IBulletWave::CollisionDetection(Bullets& bullets)
{
	Systems::CollisionSubsystem* pColSubsystem = Systems::CollisionSubsystem::GetSubsystem();
	Systems::ShapeSphere colShape(Core::Vec4f(), 1, nullptr);

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		colShape.SetCenter(bullets.m_positions[ii]);

		const Systems::ICollisionShape* pOther = nullptr;
		bool res = pColSubsystem->CollisionDetection(&colShape, &pOther);
		if (!res)
			continue;

		Systems::GameObject* pGo = pOther->GetOwner();
		if (!pGo)
			return;

		if (PlayerComponent* pPlayer = pGo->FindComponent<PlayerComponent>())
		{
			bullets.m_timeToLive[ii] = 0;
			Core::LogModule::Get().LogInfo("Bullet collided with player");
		}

		return;
	}
}

bool IBulletWave::IsAlive() const
{
	return m_isAlive;
}
