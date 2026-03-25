/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"

namespace Systems
{
	class Light
	{
	public:
		Core::Mat44f m_lightSpaceTX; // transform from world space to light space.
		Rendering::LightCBuffer m_cbuffer;
	};
}
