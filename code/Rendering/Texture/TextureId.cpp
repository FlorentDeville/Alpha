/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Texture/TextureId.h"

#include <cstdint>

namespace Rendering
{
	TextureId::TextureId()
	{
		m_id = UINT64_MAX; //invalid by default
	}

	TextureId::TextureId(size_t id)
		: m_id(id)
	{}

	bool TextureId::operator==(const TextureId id) const
	{
		return id.m_id == m_id;
	}

	bool TextureId::operator!=(const TextureId id) const
	{
		return id.m_id != m_id;
	}

	const TextureId TextureId::INVALID = TextureId(); //the default is -1
}
