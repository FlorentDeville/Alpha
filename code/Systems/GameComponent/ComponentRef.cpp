/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/ComponentRef.h"

namespace Systems
{
	ComponentRef::ComponentRef()
		: m_guid()
	{ }

	ComponentRef::ComponentRef(const Core::Guid& guid)
		: m_guid(guid)
	{ }

	ComponentRef::~ComponentRef()
	{ }
}
