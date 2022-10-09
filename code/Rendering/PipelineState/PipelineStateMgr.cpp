/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/PipelineState/PipelineStateMgr.h"

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
		id = m_pipelineStates.size();
		PipelineState* pPipelineState = new PipelineState();
		m_pipelineStates.push_back(pPipelineState);
		return pPipelineState;
	}

	void PipelineStateMgr::DeletePipelineState(PipelineStateId& id)
	{
		PipelineState* pPipelineState = m_pipelineStates[id];
		delete pPipelineState;
		m_pipelineStates[id] = nullptr;
	}

	PipelineState* PipelineStateMgr::GetPipelineState(const PipelineStateId& id) const
	{
		return m_pipelineStates[id];
	}
}
