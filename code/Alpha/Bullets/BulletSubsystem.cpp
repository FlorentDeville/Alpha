/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/BulletSubsystem.h"

#include "Alpha/Bullets/IBulletWave.h"

#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"

uint32_t BulletSubsystem::m_subsystemIndex = 0;

BulletSubsystem::BulletSubsystem()
	: Systems::ISubsystem()
	, m_waves()
{
	m_bullets.Create();
}

BulletSubsystem::~BulletSubsystem()
{
	/*for (IBulletWave* pWave : m_waves)
		delete pWave;*/

	m_bullets.Delete();
}

void BulletSubsystem::Update(const Systems::GameContext& context)
{
	float dt = context.m_pClock->GetDeltaTime();

	for (IBulletWave* pWave : m_waves)
	{
		if (!pWave)
			continue;

		pWave->Update(m_bullets, dt);
	}
}

void BulletSubsystem::BuildRenderable(Systems::RenderableScene& scene)
{
	for (IBulletWave* pWave : m_waves)
	{
		if (!pWave)
			continue;

		pWave->BuildRenderable(m_bullets, scene);
	}
}

uint32_t BulletSubsystem::AddWave(IBulletWave* pWave)
{
	uint32_t index = FindFreeSlot();
	m_waves[index] = pWave;
	return index;
}

void BulletSubsystem::RemoveWave(uint32_t index)
{
	m_waves[index] = nullptr;
}

void BulletSubsystem::InitWave(uint32_t index)
{
	m_waves[index]->Init(m_bullets);
}

void BulletSubsystem::StartWave(uint32_t index, const Core::Vec4f& pos)
{
	m_waves[index]->Start(m_bullets, pos);
}

void BulletSubsystem::StopWave(uint32_t index)
{
	m_waves[index]->Stop();
}

BulletSubsystem* BulletSubsystem::GetSubsystem()
{
	ISubsystem* pSubsystem = Systems::GameMgr::Get().GetGameSubsystem(m_subsystemIndex);
	return static_cast<BulletSubsystem*>(pSubsystem);
}

uint32_t BulletSubsystem::FindFreeSlot()
{
	uint32_t slot = m_waves.GetSize();
	for (uint32_t ii = 0; ii < slot; ++ii)
	{
		if (!m_waves[ii])
			return ii;
	}

	m_waves.PushBack(nullptr);
	return slot;
}
