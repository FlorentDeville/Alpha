/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Reflection/ReflectionGameTypes.h"

#include "Alpha/Objects/Boss/Proto/BossGameObject.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"

void RegisterGameTypesToReflection()
{
	BossGameObject::RegisterReflection();
	PlayerGameObject::RegisterReflection();
}
