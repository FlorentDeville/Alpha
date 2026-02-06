/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Tools/Color.h"

#include <cstdint>

namespace Widgets
{
	struct AlternateColorCBuffer
	{
		Color m_color1;
		Color m_color2;
		uint32_t m_size;
	};
}
