/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/PipelineState/PipelineStateMgr.h"

#include "Rendering/PipelineState/PipelineState.h"

namespace Rendering
{
	PipelineStateMgr::PipelineStateMgr()
		: Core::Singleton<PipelineStateMgr>()
		, m_pipelineStates()
	{}

	PipelineStateMgr::~PipelineStateMgr()
	{
		for (PipelineState* pPipelineState : m_pipelineStates)
		{
			delete pPipelineState;
		}

		m_pipelineStates.clear();
	}

	PipelineState* PipelineStateMgr::CreatePipelineState(PipelineStateId& id)
	{
		id = PipelineStateId(static_cast<uint32_t>(m_pipelineStates.size()));
		PipelineState* pPipelineState = new PipelineState();
		m_pipelineStates.push_back(pPipelineState);
		return pPipelineState;
	}

	void PipelineStateMgr::DeletePipelineState(PipelineStateId& id)
	{
		PipelineState* pPipelineState = m_pipelineStates[id.m_id];
		delete pPipelineState;
		m_pipelineStates[id.m_id] = nullptr;
	}

	PipelineState* PipelineStateMgr::GetPipelineState(const PipelineStateId& id) const
	{
		return m_pipelineStates[id.m_id];
	}
}
