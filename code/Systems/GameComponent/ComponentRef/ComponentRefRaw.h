/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Guid/Guid.h"

namespace Systems
{
	class ComponentRefRaw
	{
	public:
		ComponentRefRaw();
		ComponentRefRaw(const Core::Guid& guid);
		~ComponentRefRaw();

		const Core::Guid& GetGuid() const;

	private:
		Core::Guid m_guid;

		template<class T> friend class ComponentRef;
	};
}
