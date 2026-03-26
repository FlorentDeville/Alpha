/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

namespace Systems
{
	class RenderableCamera
	{
	public:
		Core::Mat44f m_view;
		Core::Mat44f m_proj;

		Core::Vec4f m_position;

		float m_fov; //in radian
	};
}
