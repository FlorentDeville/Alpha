/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/Waves/Player_Wave_Countered.h"

#include "Alpha/Bullets/Bullets.h"

#include "Core/Bezier/Bezier.h"
#include "Core/Random/Random.h"

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessageSubsystem.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"

PlayerWaveCountered::PlayerWaveCountered(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::GameObject* pOwner, 
	Systems::GameObject* pTarget)
	: IBulletWave()
	, m_pMesh(pMesh)
	, m_pMaterial(pMaterial)
	, m_pOwner(pOwner)
	, m_pTarget(pTarget)
	, m_pBullets(nullptr)
{
	m_count = 50;
}

PlayerWaveCountered::~PlayerWaveCountered()
{

}

void PlayerWaveCountered::Init(Bullets& bullets)
{
	m_pBullets = &bullets;
	m_startId = bullets.Allocate(m_count);
	m_endId = m_startId + m_count;
}

void PlayerWaveCountered::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
}

void PlayerWaveCountered::Start(Bullets& bullets)
{
	m_isAlive = true;

	//disable all bullets
	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
		bullets.m_timeToLive[ii] = -1;
}

void PlayerWaveCountered::Update(Bullets& bullets, float /*dt*/)
{
	const float LIFETIME = 0.5f;

	float currentTime = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	Core::Vec4f targetPosition = m_pTarget->GetTransform().GetWorldTx().GetT();

	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		if (bullets.m_timeToLive[ii] <= 0)
			continue;

		float param = (currentTime - bullets.m_timeToLive[ii]) / LIFETIME;

		if (param > 1)
		{
			Systems::GameMessageSubsystem* pMessage = Systems::GameMessageSubsystem::GetSubsystem();
			Systems::GameMessage msg;
			msg.m_id = CONSTSID("bullet_counter_collision");
			pMessage->SendMessage(m_pTarget, msg);

			bullets.m_timeToLive[ii] = -1;
			continue;
		}

		Core::QuadraticBezier bezier(bullets.m_speed[ii], bullets.m_acceleration[ii], targetPosition);
		bullets.m_positions[ii] = bezier.Evaluate(param);
	}
}

void PlayerWaveCountered::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
{
	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
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

//void PlayerWaveCountered::CollisionDetection(Bullets& bullet) override;

void PlayerWaveCountered::SpawnSingleBullet(const Core::Vec4f& startPos)
{
	uint32_t index = FindFreeSlot();
	if (index == UINT32_MAX)
		return;

	//time to live : the start time
	// speed : bezier p0
	// acceleration : bezier p1

	m_pBullets->m_timeToLive[index] = Systems::GameMgr::Get().GetWorld()->m_pClock->GetTime();
	m_pBullets->m_speed[index] = startPos;
	m_pBullets->m_positions[index] = startPos;

	Core::RandomUInt random(0, 1);
	int32_t side = (random.Generate() * 2) - 1;

	Core::Vec4f targetPosition = m_pTarget->GetTransform().GetWorldTx().GetT();
	Core::Vec4f startToTarget = targetPosition - startPos;
	Core::Vec4f orthogonal(startToTarget.GetZ(), 0, -startToTarget.GetX(), 0);
	orthogonal = orthogonal * static_cast<float>(side);
	Core::Vec4f p1 = startPos + (startToTarget * 0.5) + (orthogonal * 0.5);

	m_pBullets->m_acceleration[index] = p1;
	//m_pCounterBulletState[bezierIndex].m_bezierP1 = p1;
}

uint32_t PlayerWaveCountered::FindFreeSlot() const
{
	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
	{
		if (m_pBullets->m_timeToLive[ii] <= 0)
			return ii;
	}

	return UINT32_MAX;
}