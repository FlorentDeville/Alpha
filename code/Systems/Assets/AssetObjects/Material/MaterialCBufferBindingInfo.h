/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/ReflectionMacro.h"

#include <cstdint>

ENABLE_REFLECTION(Systems, MaterialCBufferBindingInfo)

namespace Systems
{
	enum CBufferType : int8_t
	{
		Invalid = -1,
		PerFrame,
		PerObject,
		PerMaterial,
		Lights
	};

	//Information about how to bind a cbuffer
	class MaterialCBufferBindingInfo
	{
	public:
		int8_t m_type; //It should be a CBufferType but the relfection doesn't support enum yet.
		int m_sigRootParamIndex;

		START_REFLECTION(Systems::MaterialCBufferBindingInfo)
			ADD_FIELD(m_type)
			ADD_FIELD(m_sigRootParamIndex)
		END_REFLECTION()
	};
}
