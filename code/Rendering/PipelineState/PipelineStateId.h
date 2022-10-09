/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Rendering
{
	class PipelineStateId
	{
		friend class PipelineStateMgr;

	public:
		PipelineStateId();
		explicit PipelineStateId(uint32_t id);

		bool operator==(PipelineStateId id) const;

		static const PipelineStateId INVALID;

	private:
		uint32_t m_id;
	};
}
