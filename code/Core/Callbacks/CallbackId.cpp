/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackId.h"

namespace Core
{
	CallbackId::CallbackId()
		: m_id(-1)
	{}

	CallbackId::CallbackId(uint64_t id)
		: m_id(id)
	{}

	CallbackId::~CallbackId()
	{}

	bool CallbackId::IsValid() const
	{
		return m_id != -1;
	}

	bool CallbackId::operator==(const CallbackId other) const
	{
		return m_id == other.m_id;
	}

	bool CallbackId::operator<(const CallbackId other) const
	{
		return m_id < other.m_id;
	}
}
