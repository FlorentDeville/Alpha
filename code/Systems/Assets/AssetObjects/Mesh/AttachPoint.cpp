/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Mesh/AttachPoint.h"

namespace Systems
{
	const std::string& AttachPoint::GetName() const
	{
		return m_name;
	}

	const Core::Sqt& AttachPoint::GetLocator() const
	{
		return m_locator;
	}

	void AttachPoint::UpdateNameSid()
	{
		m_nameSid = Core::MakeSid(m_name);
	}
}
