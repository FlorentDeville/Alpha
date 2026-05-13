/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"
#include "Systems/Objects/GameObject.h"

namespace Rendering
{
	class Camera;
}

ENABLE_GAME_REFLECTION(PlayerGameObject)

class PlayerGameObject : public Systems::GameObject
{
	using BaseClass = Systems::GameObject;

public:
	PlayerGameObject();
	~PlayerGameObject();

	void OnStartGame() override;

	void Update(float dt) override;

	void OnDestroyGame() override;

private:
	float m_speed;
	Core::Vec4f m_cameraOffset;

	START_REFLECTION(PlayerGameObject)
		ADD_BASETYPE(Systems::GameObject)
		ADD_FIELD(m_speed)
		ADD_FIELD(m_cameraOffset)
	END_REFLECTION()

	Rendering::Camera* m_pCamera;
};
