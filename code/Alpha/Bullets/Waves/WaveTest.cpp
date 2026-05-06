/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/Waves/WaveTest.h"

#include "Alpha/Bullets/Bullets.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Vec4f.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderView.h"

#include <assert.h>
#include <cmath>

WaveTest::WaveTest(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial)
	: IBulletWave()
{
	m_count = 10;
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;
}

WaveTest::~WaveTest()
{ }

void WaveTest::Spawn(Bullets& bullets, const Core::Vec4f& pos)
{
	//allocate bullets
	m_startId = bullets.Allocate(m_count);
	assert(m_startId != UINT32_MAX);
	m_endId = m_startId + m_count;

	//now spawn them with their initial values
	// make a basic pattern where the spawn shape is a circle and bullets go in a straight line

	const float ANGLE_INC = Core::TWO_PI / m_count;
	const float CIRCLE_RADIUS = 1.f;
	const float SPEED = 5.f;
	for (uint32_t ii = 0; ii < m_count; ++ii)
	{
		float angle = ii * ANGLE_INC;
		float x = cos(angle);
		float z = sin(angle);

		bullets.m_positions[m_startId + ii] = pos + Core::Vec4f(x, 0, z, 0) * CIRCLE_RADIUS;
		bullets.m_speed[m_startId + ii] = Core::Vec4f(x, 0, z, 0) * SPEED;
		bullets.m_acceleration[m_startId + ii] = Core::Vec4f(0, 0, 0, 0);
		bullets.m_timeToLive[m_startId + ii] = 5;
	}
}

void WaveTest::Update(Bullets& bullets, float dt)
{
	//reduce ttl
	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
		bullets.m_timeToLive[ii] = bullets.m_timeToLive[ii] - dt;

	//compute new position
	for (uint32_t ii = m_startId; ii < m_endId; ++ii)
		bullets.m_positions[ii] = bullets.m_positions[ii] + bullets.m_speed[ii] * dt;
}

void WaveTest::BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene)
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
