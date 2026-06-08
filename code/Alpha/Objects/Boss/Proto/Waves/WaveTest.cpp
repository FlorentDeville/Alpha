/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Proto/Waves/WaveTest.h"

#include "Alpha/Bullets/Bullets.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Vec4f.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderView.h"

#include <assert.h>
#include <cmath>

WaveTest::WaveTest(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount, float rotationOffset)
	: IBulletWave()
	, m_showDuration(0.5f)
	, m_showTime(0)
	, m_currentState(State::SHOW)
	, m_currentScale(0)
	, m_rotationOffset(rotationOffset)
	, m_startPosition()
{
	m_count = bulletCount;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;
}

WaveTest::~WaveTest()
{ }

void WaveTest::Init(Bullets& bullets)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;
}

void WaveTest::Destroy(Bullets& bullets)
{
	bullets.Free(m_startId, m_count);
}

void WaveTest::Start(Bullets& bullets, const Core::Vec4f& /*pos*/)
{
	// make a basic pattern where the spawn shape is a circle and bullets go in a straight line
	const float ANGLE_INC = Core::TWO_PI / m_count;
	const float CIRCLE_RADIUS = 2.f;
	const float SPEED = 35.f;
	for (uint32_t ii = 0; ii < m_count; ++ii)
	{
		float angle = (ii * ANGLE_INC) + m_rotationOffset;
		float x = cos(angle);
		float z = sin(angle);

		bullets.m_positions[m_startId + ii] = m_startPosition + Core::Vec4f(x, 0, z, 0) * CIRCLE_RADIUS;
		bullets.m_speed[m_startId + ii] = Core::Vec4f(x, 0, z, 0) * SPEED;
		bullets.m_acceleration[m_startId + ii] = Core::Vec4f(0, 0, 0, 0);
		bullets.m_timeToLive[m_startId + ii] = 2;
		bullets.m_type[m_startId + ii] = BulletType::NORMAL;
		bullets.m_state[m_startId + ii] = BulletState::ATTACK;
	}

	m_isAlive = true;
	m_currentState = State::SHOW;
	m_showTime = 0;
	m_currentScale = 0;
}

void WaveTest::Update(Bullets& bullets, float dt)
{
	if (!m_isAlive)
		return;

	switch (m_currentState)
	{
	case State::SHOW:
	{
		m_showTime += dt;
		if (m_showTime >= m_showDuration)
		{
			m_currentState = State::FIRE;
			break;
		}

		m_currentScale = m_showTime / m_showDuration;
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
	}
	break;

	}
}

void WaveTest::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
{
	if (!m_isAlive)
		return;

	m_isAlive = false;

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

		m_isAlive = true;
	}
}

void WaveTest::SetStartPosition(const Core::Vec4f& startPos)
{
	m_startPosition = startPos;
}