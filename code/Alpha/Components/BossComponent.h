/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/GameComponent.h"

ENABLE_GAME_REFLECTION(BossComponent)

namespace Systems
{
	class World;
}

class BossComponent : public Systems::GameComponent
{
public:
	BossComponent();
	~BossComponent();

	void PostLoad() override;

	void OnStart(Systems::World* pWorld) override;
	void Update(float dt) override;
	void OnDestroy(Systems::World* pWorld) override;

private:


	START_REFLECTION(BossComponent)
		ADD_BASETYPE(Systems::GameComponent)
	END_REFLECTION()
};
