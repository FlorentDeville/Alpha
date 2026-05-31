/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

namespace Systems
{
	class GameMessage
	{
	public:
		Core::Sid m_id;
		uint64_t m_param;

		template<typename T> void SetParameter(const T& param)
		{
			m_param = reinterpret_cast<T>(m_param);
		}

		template<typename T> const T GetParameter() const
		{
			return reinterpret_cast<T>(m_param);
		}
	};
}
