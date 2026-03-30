/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/GameComponent.h"

ENABLE_GAME_REFLECTION(PlayerComponent)

namespace Rendering
{
	class Camera;
}

class PlayerComponent : public Systems::GameComponent
{
public:
	PlayerComponent();
	~PlayerComponent();

	void PostLoad() override;
	void Update(float dt) override;

private:

	float m_speed;

	START_REFLECTION(PlayerComponent)
		ADD_BASETYPE(Systems::GameComponent)
		ADD_FIELD(m_speed)
	END_REFLECTION()

	Rendering::Camera* m_pCamera;
	bool m_isCameraRegistered;
};
