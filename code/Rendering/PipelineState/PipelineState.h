/********************************************************************/
/* � 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/RootSignature/RootSignatureId.h"
#include "Rendering/Shaders/ShaderId.h"

struct ID3D12PipelineState;

namespace Rendering
{
	class RootSignature;
	class Shader;

	class PipelineState
	{
	public:
		PipelineState();
		~PipelineState();

		void Init_Pos(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
		void Init_PosUv(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
		void Init_PosColor(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
		void Init_Text(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
		void Init_Icon(RootSignatureId rsId, ShaderId vsId, ShaderId psId);

		//this is the generic pipeline state for meshes containing every vertex attributes
		void Init_Generic(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
		void Init_Generic(const RootSignature& rs, const Shader& vs, const Shader& ps);

		ID3D12PipelineState* GetPipelineState() const;
		RootSignatureId GetRootSignatureId() const;

	private:
		ID3D12PipelineState* m_pPipelineState;
		RootSignatureId m_rsId;
		ShaderId m_vsId;
		ShaderId m_psId;
	};
}
