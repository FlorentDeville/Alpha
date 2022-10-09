/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/PipelineState/PipelineStateId.h"

namespace Rendering
{
	PipelineStateId::PipelineStateId()
		: m_id(UINT32_MAX)
	{}

	PipelineStateId::PipelineStateId(uint32_t id)
		: m_id(id)
	{}

	bool PipelineStateId::operator==(PipelineStateId id) const
	{
		return id.m_id == m_id;
	}

	const PipelineStateId INVALID = PipelineStateId();
}
