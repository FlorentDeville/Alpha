/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Reflection/ReflectionGameTypes.h"

#include "Alpha/Components/Boss/BossComponent.h"
#include "Alpha/Components/PlayerComponent.h"
#include "Alpha/Objects/PlayerGameObject.h"

void RegisterGameTypesToReflection()
{
	BossComponent::RegisterReflection();
	PlayerComponent::RegisterReflection();

	PlayerGameObject::RegisterReflection();
}
