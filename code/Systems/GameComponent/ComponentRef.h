/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Guid/Guid.h"
#include "Core/Reflection/ReflectionMacro.h"

ENABLE_REFLECTION(Systems, ComponentRef)

namespace Systems
{
	class ComponentRef
	{
	public:
		ComponentRef();
		ComponentRef(const Core::Guid& guid);
		~ComponentRef();

	private:
		Core::Guid m_guid;

		START_REFLECTION(Systems::ComponentRef)
			ADD_FIELD(m_guid)
		END_REFLECTION()
	};
}
