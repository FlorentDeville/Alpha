/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/ComponentRef/ComponentRefRaw.h"

namespace Systems
{
	ComponentRefRaw::ComponentRefRaw()
		: m_guid()
	{ }

	ComponentRefRaw::ComponentRefRaw(const Core::Guid& guid)
		: m_guid(guid)
	{ }

	ComponentRefRaw::~ComponentRefRaw()
	{ }
}
