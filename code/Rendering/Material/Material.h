/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once
#define NOMINMAX
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/Texture/TextureId.h"

namespace Rendering
{
	class RootSignature;
	struct RootSignatureId;

	class Material
	{
		friend class RenderModule;

	public:
		Material();
		~Material();

		bool Init(RootSignatureId rid, PipelineStateId pid);

		void SetTexture(Rendering::TextureId id);

	private:
		RootSignature* m_pRootSignature;
		PipelineState* m_pPipelineState;

		Rendering::TextureId m_textureId;
		//constant buffer
	};
}
