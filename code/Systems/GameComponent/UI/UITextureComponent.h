/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vectors.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Objects/GameComponent.h"

ENABLE_REFLECTION(Systems, UITextureComponent)

namespace Systems
{
	class Texture2DAsset;

	class UITextureComponent : public GameComponent
	{
	public:
		UITextureComponent();
		~UITextureComponent();

	private:
		std::string m_debugName;
		HardAssetRef<Texture2DAsset> m_texture;
		Core::Float2 m_position;
		Core::Float2 m_scale;

		START_REFLECTION(Systems::UITextureComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_debugName)
			ADD_FIELD(m_texture)
			ADD_FIELD(m_position)
			ADD_FIELD(m_scale)
		END_REFLECTION()
	};
}

