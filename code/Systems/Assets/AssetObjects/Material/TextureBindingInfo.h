/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, TextureBindingInfo)
	class TextureBindingInfo
	{
	public:
		//std::string m_name;
		//uint32_t m_size;
		//uint32_t m_offset;
		//Core::Sid m_type;
		//std::string m_strType;

		////This is the value of this parameter, saved as a binary blob
		//Core::Array<char> m_value;

		//START_REFLECTION(Systems::MaterialParameterDescription)
		//	ADD_FIELD(m_name)
		//	ADD_FIELD(m_size)
		//	ADD_FIELD(m_offset)
		//	ADD_FIELD(m_type)
		//	ADD_FIELD(m_strType)
		//	ADD_FIELD(m_value)
		//END_REFLECTION()
	};
}
