/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/RootSignature/RootSignatureId.h"
#include "Rendering/ShaderId.h"
#include "Resource/ResourceMgr.h"

#include <d3d12.h>

class PipelineState : public Resource
{
public:
	PipelineState(const std::string& name);
	~PipelineState();

	void Init_Pos(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
	void Init_PosUv(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
	void Init_PosColor(RootSignatureId rsId, ShaderId vsId, ShaderId psId);

	ID3D12PipelineState* GetPipelineState() const;
	RootSignatureId GetRootSignatureId() const;

private:
	ID3D12PipelineState* m_pPipelineState;
	RootSignatureId m_rsId;
	ShaderId m_vsId;
	ShaderId m_psId;
};

DECLARE_RESOURCE_ID(PipelineState);
extern RESOURCE_MGR_PTR(PipelineState) g_pPipelineStateMgr;
