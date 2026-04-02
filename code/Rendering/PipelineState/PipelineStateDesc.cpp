/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/PipelineState/PipelineStateDesc.h"

namespace Rendering
{
	PipelineStateDesc::PipelineStateDesc()
		: m_pRs(nullptr)
		, m_pVs(nullptr)
		, m_pPs(nullptr)
		, m_cullMode(CullMode::Back)
		, m_depthFunction(DepthComparisonMode::Less)
		, m_rtvFormat(ResourceFormat::R8G8B8A8_UNORM_SRGB)
	{ }
}
