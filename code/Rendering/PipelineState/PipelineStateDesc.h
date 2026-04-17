/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/PipelineState/BlendFactor.h"
#include "Rendering/PipelineState/BlendOperation.h"
#include "Rendering/PipelineState/CullMode.h"
#include "Rendering/PipelineState/DepthComparisonMode.h"
#include "Rendering/BufferFormat.h"

namespace Rendering
{
	class RootSignature;
	class Shader;

	//!!Logic operation not implemented
	class BlendDesc
	{
	public:
		BlendDesc();
		~BlendDesc() = default;

		bool m_blendEnabled;
		BlendFactor m_srcBlend; //factor to apply to the pixel shader output
		BlendFactor m_dstBlend; //factor to apply to the current value in the render target
		BlendOperation m_blendOperation;

		BlendFactor m_srcBlendAlpha; //factor to apply to the pixel shader alpha channel output
		BlendFactor m_dstBlendAlpha; //factor to apply to the current render target value alpha channel
		BlendOperation m_blendOperationAlpha;
	};

	class DepthStencilDesc
	{
	public:
		DepthStencilDesc();
		~DepthStencilDesc() = default;

		DepthComparisonMode m_depthFunction;
	};

	class PipelineStateDesc
	{
	public:
		PipelineStateDesc();
		~PipelineStateDesc() = default;

		RootSignature* m_pRs;
		Shader* m_pVs;
		Shader* m_pPs;
		CullMode m_cullMode;
		
		BufferFormat m_rtvFormat;
		BlendDesc m_blendDesc; //only a single render target is supported
		DepthStencilDesc m_depthStencilDesc;
	};
}
