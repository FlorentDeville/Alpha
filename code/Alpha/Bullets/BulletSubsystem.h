/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/Bullets.h"

#include "Core/Collections/Array.h"

#include "Systems/Game/Subsystems/ISubsystem.h"

namespace Systems
{
	class RenderableScene;
}

class IBulletWave;

class BulletSubsystem : public Systems::ISubsystem
{
public:
	BulletSubsystem();
	~BulletSubsystem();

	void Update(const Systems::GameContext& context) override;
	void BuildRenderable(Systems::RenderableScene& scene) override;

	uint32_t AddWave(IBulletWave* pWave);
	void RemoveWave(uint32_t index);
	void InitWave(uint32_t index);
	void StartWave(uint32_t index, const Core::Vec4f& pos);

	static uint32_t m_subsystemIndex;
	static BulletSubsystem* GetSubsystem();

private:
	Core::Array<IBulletWave*> m_waves;

	Bullets m_bullets;

	uint32_t FindFreeSlot();
};
