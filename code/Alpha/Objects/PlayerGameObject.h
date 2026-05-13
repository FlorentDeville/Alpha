/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"
#include "Systems/Objects/GameObject.h"

ENABLE_GAME_REFLECTION(PlayerGameObject)

class PlayerGameObject : public Systems::GameObject
{
public:
	PlayerGameObject();
	~PlayerGameObject() = default;

private:
	uint32_t m_test;

	START_REFLECTION(PlayerGameObject)
		ADD_BASETYPE(Systems::GameObject)
		ADD_FIELD(m_test)
	END_REFLECTION()
};
