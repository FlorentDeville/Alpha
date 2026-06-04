/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Reflection/ReflectionGameTypes.h"

#include "Alpha/Objects/Boss/BaseBoss.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"
#include "Alpha/Objects/Boss/Proto/BossGameObject.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"

void RegisterGameTypesToReflection()
{
	BaseBoss::RegisterReflection();
	BossGameObject::RegisterReflection();
	Ichi::RegisterReflection();
	PlayerGameObject::RegisterReflection();
}
