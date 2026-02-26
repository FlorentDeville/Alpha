/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetObjects/Texture/TextureAsset.h"
#include "Systems/Assets/AssetRef/HardAssetRef.h"

namespace Systems
{

	ENABLE_REFLECTION_WITH_NS(Systems, TextureBindingInfo)
	class TextureBindingInfo
	{
	public:
		HardAssetRef<TextureAsset> m_texture;
		std::string m_name;
		uint32_t m_sigRootIndex;
		
		START_REFLECTION(Systems::TextureBindingInfo)
			ADD_FIELD(m_texture)
			ADD_FIELD_ATTR(m_name, ReadOnly)
			ADD_FIELD_ATTR(m_sigRootIndex, ReadOnly)
		END_REFLECTION()
	};
}
