/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Reflection/ReflectionRenderingTypes.h"

#include "Rendering/PipelineState/CullMode.h"
#include "Rendering/PipelineState/DepthComparisonMode.h"

namespace Rendering
{
	void RegisterRenderingTypesToReflection()
	{
		CullMode_RegisterReflection();
		DepthComparisonMode_RegisterReflection();
	}
}
