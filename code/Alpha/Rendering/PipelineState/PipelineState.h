/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/RootSignature/RootSignatureId.h"
#include "Rendering/ShaderId.h"

#include <d3d12.h>

class PipelineState
{
public:
	PipelineState();
	~PipelineState();

	void Init_PosColor(RootSignatureId rsId, ShaderId vsId, ShaderId psId);

	ID3D12PipelineState* GetPipelineState() const;
	RootSignatureId GetRootSignatureId() const;

private:
	ID3D12PipelineState* m_pPipelineState;
	RootSignatureId m_rsId;
	ShaderId m_vsId;
	ShaderId m_psId;
};
