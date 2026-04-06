/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vectors.h"

namespace Rendering
{
	class PerFrameCBuffer
	{
	public:
		PerFrameCBuffer(const Core::Mat44f& view, const Core::Mat44f& proj, const Core::Float3& cameraPos, float time);

	private:
		Core::Mat44f m_viewMatrix;
		Core::Mat44f m_projMatrix;
		Core::Float3 m_cameraPosition;
		float m_time;	// in seconds
	};
}
