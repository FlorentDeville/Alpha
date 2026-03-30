/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Reflection/ReflectionGameTypes.h"

#include "Alpha/Components/PlayerComponent.h"

void RegisterGameTypesToReflection()
{
	PlayerComponent::RegisterReflection();
}
