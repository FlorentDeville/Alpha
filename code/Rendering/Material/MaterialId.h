/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Rendering
{
	class MaterialId
	{
		friend class MaterialMgr;

	public:
		static const MaterialId INVALID;

		MaterialId();
		explicit MaterialId(uint32_t id);

	private:
		uint32_t m_id;
	};
}
