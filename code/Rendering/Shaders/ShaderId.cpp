/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Shaders/ShaderId.h"

const ShaderId ShaderId::INVALID = ShaderId();

ShaderId::ShaderId()
	: m_id(UINT32_MAX)
{}

ShaderId::ShaderId(uint32_t id)
	: m_id(id)
{}

ShaderId::ShaderId(size_t id)
	: m_id(static_cast<uint32_t>(id))
{}

bool ShaderId::operator==(const ShaderId id)
{
	return id.m_id == m_id;
}

