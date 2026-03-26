/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Rendering/Renderable/RenderableCamera.h"
#include "Systems/Rendering/Renderable/RenderableLight.h"
#include "Systems/Rendering/Renderable/RenderableObject.h"

namespace Systems
{
	class LevelAsset;

	class RenderableObject;
	class RenderableLight;

	class RenderableScene
	{
	public:
		Core::Array<RenderableObject> m_objects;
		Core::Array<RenderableLight> m_lights;

		RenderableCamera m_camera;
	};

	void PrepareRenderableCamera(const Core::Mat44f& view, const Core::Mat44f& proj, const Core::Vec4f& position, float fov, RenderableScene& scene);
	void PrepareRenderableScene(const LevelAsset* pLevel, RenderableScene& scene);
}
