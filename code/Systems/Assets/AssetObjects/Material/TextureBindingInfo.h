/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetObjects/Texture/TextureAsset.h"
#include "Systems/Assets/AssetRef/HardAssetRef.h"

ENABLE_REFLECTION(Systems, TextureBindingInfo)

namespace Systems
{
	class TextureBindingInfo
	{
	public:
		HardAssetRef<TextureAsset> m_texture;
		std::string m_name;
		uint32_t m_sigRootIndex;
		
		START_REFLECTION(Systems::TextureBindingInfo)
			ADD_FIELD(m_texture)
			ADD_FIELD_ATTR(m_name, Core::ReadOnly)
			ADD_FIELD_ATTR(m_sigRootIndex, Core::ReadOnly)
		END_REFLECTION()
	};
}
