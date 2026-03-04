/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Reflection/ReflectionRenderingTypes.h"

#include "Rendering/PipelineState/CullMode.h"

namespace Rendering
{
	void RegisterRenderingTypesToReflection()
	{
		CullMode_RegisterReflection();
	}
}
