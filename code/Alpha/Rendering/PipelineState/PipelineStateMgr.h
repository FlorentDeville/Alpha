/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/RootSignature/RootSignatureId.h"
#include "Rendering/ShaderId.h"

#include <vector>

class PipelineState;

class PipelineStateMgr
{
public:
	PipelineStateMgr();
	~PipelineStateMgr();

	PipelineStateId Create_PosColor(RootSignatureId rsId, ShaderId vsId, ShaderId psId);
	PipelineState* GetPipelineState(PipelineStateId id) const;

private:
	std::vector<PipelineState*> m_pipelineStates;

};

extern PipelineStateMgr* g_pPipelineStateMgr;
