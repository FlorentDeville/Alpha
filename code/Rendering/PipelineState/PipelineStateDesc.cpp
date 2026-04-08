/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/PipelineState/PipelineStateDesc.h"

namespace Rendering
{
	BlendDesc::BlendDesc()
		: m_blendEnabled(false)
		, m_srcBlend(BlendFactor::ONE)
		, m_dstBlend(BlendFactor::ZERO)
		, m_blendOperation(BlendOperation::ADD)
		, m_srcBlendAlpha(BlendFactor::ONE)
		, m_dstBlendAlpha(BlendFactor::ZERO)
		, m_blendOperationAlpha(BlendOperation::ADD)
	{ }

	PipelineStateDesc::PipelineStateDesc()
		: m_pRs(nullptr)
		, m_pVs(nullptr)
		, m_pPs(nullptr)
		, m_cullMode(CullMode::BACK)
		, m_depthFunction(DepthComparisonMode::LESS)
		, m_rtvFormat(BufferFormat::R16G16B16A16_FLOAT)
		, m_blendDesc()
	{ }
}
