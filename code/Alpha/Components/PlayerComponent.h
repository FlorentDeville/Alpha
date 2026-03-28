/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/GameComponent.h"

ENABLE_GAME_REFLECTION(PlayerComponent)

class PlayerComponent : public Systems::GameComponent
{
public:
	PlayerComponent();
	~PlayerComponent();

	void Update() override;

private:

	float m_speed;

	START_REFLECTION(PlayerComponent)
		ADD_BASETYPE(Systems::GameComponent)
		ADD_FIELD(m_speed)
	END_REFLECTION()
};
