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
	m_bullets.Create();
}

BulletSubsystem::~BulletSubsystem()
{
	for (IBulletWave* pWave : m_waves)
		delete pWave;

	m_bullets.Delete();
}

void BulletSubsystem::Update(const Systems::GameContext& context)
{
	float dt = context.m_pClock->GetDeltaTime();

	for (IBulletWave* pWave : m_waves)
		pWave->Update(m_bullets, dt);
}

void BulletSubsystem::BuildRenderable(Systems::RenderableScene& /*scene*/)
{ }

uint32_t BulletSubsystem::AddWave(IBulletWave* pWave)
{
	uint32_t index = m_waves.GetSize();
	m_waves.PushBack(pWave);
	return index;
}
