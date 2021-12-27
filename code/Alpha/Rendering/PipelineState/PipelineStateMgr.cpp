/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/PipelineState/PipelineStateMgr.h"

#include "Rendering/PipelineState/PipelineState.h"

#include <assert.h>

PipelineStateMgr::PipelineStateMgr()
	: m_pipelineStates()
{}

PipelineStateMgr::~PipelineStateMgr()
{
	for (PipelineState* pState : m_pipelineStates)
		delete pState;
}

PipelineStateId PipelineStateMgr::Create_Pos(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
{
	PipelineStateId id;
	id.m_id = m_pipelineStates.size();

	PipelineState* pPipelineState = new PipelineState();
	pPipelineState->Init_Pos(rsId, vsId, psId);
	m_pipelineStates.push_back(pPipelineState);

	return id;
}

PipelineStateId PipelineStateMgr::Create_PosUv(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
{
	PipelineStateId id;
	id.m_id = m_pipelineStates.size();

	PipelineState* pPipelineState = new PipelineState();
	pPipelineState->Init_PosUv(rsId, vsId, psId);
	m_pipelineStates.push_back(pPipelineState);

	return id;
}

PipelineStateId PipelineStateMgr::Create_PosColor(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
{
	PipelineStateId id;
	id.m_id = m_pipelineStates.size();

	PipelineState* pPipelineState = new PipelineState();
	pPipelineState->Init_PosColor(rsId, vsId, psId);
	m_pipelineStates.push_back(pPipelineState);

	return id;
}

PipelineState* PipelineStateMgr::GetPipelineState(PipelineStateId id) const
{
	assert(id.m_id < m_pipelineStates.size());

	return m_pipelineStates[id.m_id];
}

PipelineStateMgr* g_pPipelineStateMgr = nullptr;
