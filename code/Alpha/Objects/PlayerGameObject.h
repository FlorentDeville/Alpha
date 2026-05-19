/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/GameComponent/ComponentRef/ComponentRef.h"
#include "Systems/GameComponent/UI/UIBaseComponent.h"
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
	void PostUpdate() override;

	void HandleMessage(const Systems::GameMessage& msg) override;

	void OnDestroyGame() override;

private:
	float m_speed;
	Core::Vec4f m_cameraOffset;

	uint32_t m_maxHp;

	Systems::ComponentRef<Systems::UIBaseComponent> m_currentHealthComp;
	Systems::ComponentRef<Systems::UIBaseComponent> m_totalHealthComp;

	START_REFLECTION(PlayerGameObject)
		ADD_BASETYPE(Systems::GameObject)
		ADD_FIELD(m_speed)
		ADD_FIELD(m_cameraOffset)
		ADD_FIELD(m_maxHp)
		ADD_FIELD(m_currentHealthComp)
		ADD_FIELD(m_totalHealthComp)
	END_REFLECTION()

	Rendering::Camera* m_pCamera;

	uint32_t m_currentHp;

	//Index of the counter bullet the player collided with
	uint32_t m_counterBulletIndex;

	void OnBulletCollision();

	void UpdateCamera();
};
