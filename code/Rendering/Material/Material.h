/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/PipelineState/PipelineState.h"

class RootSignature;

struct RootSignatureId;

namespace Rendering
{
	class Material
	{
		friend class RenderModule;

	public:
		Material();
		~Material();

		bool Init(RootSignatureId rid, PipelineStateId pid);

	private:
		RootSignature* m_pRootSignature;
		PipelineState* m_pPipelineState;
		//constant buffer
	};
}
