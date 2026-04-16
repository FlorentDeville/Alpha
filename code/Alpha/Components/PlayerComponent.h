/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

#include "Systems/Objects/GameComponent.h"

ENABLE_GAME_REFLECTION(PlayerComponent)

namespace Rendering
{
	class Camera;
}

namespace Systems
{
	class World;
}

class PlayerComponent : public Systems::GameComponent
{
public:
	PlayerComponent();
	~PlayerComponent();

	void PostLoad() override;

	void OnStart(Systems::World* pWorld) override;
	void Update(float dt) override;
	void OnDestroy(Systems::World* pWorld) override;

private:

	float m_speed;
	Core::Vec4f m_cameraOffset;

	START_REFLECTION(PlayerComponent)
		ADD_BASETYPE(Systems::GameComponent)
		ADD_FIELD(m_speed)
		ADD_FIELD(m_cameraOffset)
	END_REFLECTION()

	Rendering::Camera* m_pCamera;
};
