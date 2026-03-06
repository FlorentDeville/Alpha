/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/PipelineState/CullMode.h"

namespace Rendering
{
	class RootSignature;
	class Shader;

	class PipelineStateDesc
	{
	public:
		PipelineStateDesc();
		~PipelineStateDesc() = default;

		RootSignature* m_pRs;
		Shader* m_pVs;
		Shader* m_pPs;
		CullMode m_cullMode;
	};
}
