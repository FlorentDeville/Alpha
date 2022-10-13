/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Rendering
{
	class ShaderId
	{
		friend class ShaderMgr;

	public:
		ShaderId();
		explicit ShaderId(uint32_t id);
		explicit ShaderId(size_t id);

		bool operator==(const ShaderId id);

		static const ShaderId INVALID;

	private:
		uint32_t m_id;
	};
}
