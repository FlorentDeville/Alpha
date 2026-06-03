/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Sqt.h"
#include "Core/Reflection/ReflectionMacro.h"
#include "Core/Sid/Sid.h"

#include <string>

ENABLE_REFLECTION(Systems, AttachPoint)

namespace Systems
{
	class AttachPoint
	{
	public:
		AttachPoint() = default;
		~AttachPoint() = default;

		const std::string& GetName() const;
		const Core::Sid GetNameSid() const;
		const Core::Sqt& GetLocator() const;
		Core::Sqt& GetLocator();

		void UpdateNameSid();

	private:
		Core::Sqt m_locator;
		std::string m_name;
		Core::Sid m_nameSid;

		START_REFLECTION(Systems::AttachPoint)
			ADD_FIELD(m_locator)
			ADD_FIELD(m_name)
			ADD_FIELD_ATTR(m_nameSid, Core::ReadOnly)
		END_REFLECTION()
	};
}
