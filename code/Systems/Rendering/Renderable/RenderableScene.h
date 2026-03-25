/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

namespace Systems
{
	class RenderableObject;
	class RenderableLight;

	class RenderableScene
	{
	public:
		Core::Array<RenderableObject> m_objects;
		Core::Array<RenderableLight> m_lights;
	};
}
