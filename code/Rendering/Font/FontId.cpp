/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Font/FontId.h"

#include <cstdint>

namespace Rendering
{
	FontId::FontId()
	{
		m_id = UINT64_MAX; //invalid by default
	}

	FontId::FontId(size_t id)
		: m_id(id)
	{}

	bool FontId::operator==(const FontId id) const
	{
		return id.m_id == m_id;
	}

	bool FontId::operator<(const FontId id) const
	{
		return m_id < id.m_id;
	}

	const FontId FontId::INVALID = FontId(); //the default is -1
}
