/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/BulletSubsystem.h"

#include "Alpha/Bullets/IBulletWave.h"

#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/GameContext.h"

BulletSubsystem::BulletSubsystem()
	: Systems::ISubsystem()
	, m_waves()
{
	const int MAX_BULLETS_COUNT = 1000;
	m_bullets.m_positions = new Core::Vec4f[MAX_BULLETS_COUNT];
	m_bullets.m_speed = new Core::Vec4f[MAX_BULLETS_COUNT];
	m_bullets.m_acceleration = new Core::Vec4f[MAX_BULLETS_COUNT];
	m_bullets.m_timeToLive = new float[MAX_BULLETS_COUNT];
	m_bullets.m_alive = new bool[MAX_BULLETS_COUNT];
}

BulletSubsystem::~BulletSubsystem()
{
	for (IBulletWave* pWave : m_waves)
		delete pWave;

	delete[] m_bullets.m_positions;
	delete[] m_bullets.m_speed;
	delete[] m_bullets.m_acceleration;
	delete[] m_bullets.m_alive;
	delete[] m_bullets.m_timeToLive;
}

void BulletSubsystem::Update(const Systems::GameContext& context)
{
	float dt = context.m_pClock->GetDeltaTime();

	for (IBulletWave* pWave : m_waves)
		pWave->Update(dt);
}

void BulletSubsystem::BuildRenderable(Systems::RenderableScene& /*scene*/)
{ }

uint32_t BulletSubsystem::AddWave(IBulletWave* pWave)
{
	uint32_t index = m_waves.GetSize();
	m_waves.PushBack(pWave);
	return index;
}
