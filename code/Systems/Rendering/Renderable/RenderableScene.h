/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Rendering/Renderable/RenderableCamera.h"
#include "Systems/Rendering/Renderable/RenderableLight.h"	//don't forward declare, I'll have to include it anyway
#include "Systems/Rendering/Renderable/RenderableObject.h"	//don't forward declare, I'll have to include it anyway

namespace Systems
{
	class LevelAsset;

	class RenderableScene
	{
	public:
		Core::Array<RenderableObject> m_objects;
		Core::Array<RenderableLight> m_lights;

		RenderableCamera m_camera;

		float m_time;
	};

	void PrepareRenderableCamera(const Core::Mat44f& view, const Core::Mat44f& proj, const Core::Vec4f& position, float fov, RenderableScene& scene);
	void PrepareRenderableScene(const LevelAsset* pLevel, RenderableScene& scene);
}
