/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Rendering/PipelineState/PipelineStateId.h"

#include <vector>

class PipelineState;

namespace Rendering
{
	class PipelineStateMgr : public Core::Singleton<PipelineStateMgr>
	{
	public:
		PipelineStateMgr();
		~PipelineStateMgr();

		PipelineState* CreatePipelineState(PipelineStateId& id);
		void DeletePipelineState(PipelineStateId& id);

		PipelineState* GetPipelineState(const PipelineStateId& id) const;

	private:
		std::vector<PipelineState*> m_pipelineStates;
	};
}
