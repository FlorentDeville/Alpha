/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"

namespace Rendering
{
	// This is the structure repsrenting the per object const buffer. It needs to match the structure passed to shaders
	// and defined in includes\CBufferPerObject.hlsl.
	class PerObjectCBuffer
	{
	public:
		PerObjectCBuffer();

		Core::Mat44f m_world;
		Core::Mat44f m_lightSpaceMatrix[LightsArrayCBuffer::MAX_LIGHT_COUNT];
	};
}
