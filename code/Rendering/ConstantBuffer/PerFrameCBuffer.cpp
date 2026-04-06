/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"

namespace Rendering
{
	PerFrameCBuffer::PerFrameCBuffer(const Core::Mat44f& view, const Core::Mat44f& proj, const Core::Float3& cameraPos, float time)
		: m_viewMatrix(view)
		, m_projMatrix(proj)
		, m_cameraPosition(cameraPos)
		, m_time(time)
	{ }
}
