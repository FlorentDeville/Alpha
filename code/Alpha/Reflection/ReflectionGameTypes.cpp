/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Reflection/ReflectionGameTypes.h"

#include "Alpha/Objects/BossGameObject.h"
#include "Alpha/Objects/PlayerGameObject.h"

void RegisterGameTypesToReflection()
{
	BossGameObject::RegisterReflection();
	PlayerGameObject::RegisterReflection();
}
