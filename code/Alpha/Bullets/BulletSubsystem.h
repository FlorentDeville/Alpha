/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Game/Subsystems/ISubsystem.h"

namespace Systems
{
	class RenderableScene;
}

class BulletSubsystem : public Systems::ISubsystem
{
public:
	BulletSubsystem();
	~BulletSubsystem();

	void Update(const Systems::GameContext& context) override;
	void BuildRenderable(Systems::RenderableScene& scene) override;
};
