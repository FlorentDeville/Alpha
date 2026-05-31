/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vectors.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Objects/GameComponent.h"

ENABLE_REFLECTION(Systems, UIBaseComponent)

namespace Systems
{
	class Texture2DAsset;

	class UIBaseComponent : public GameComponent
	{
	public:
		UIBaseComponent();
		~UIBaseComponent();

		const Texture2DAsset* GetTexture() const;

		const Core::Float2 GetPosition() const;
		const Core::Float2 GetSize() const;

		void SetPosition(const Core::Float2& pos);
		void SetSize(const Core::Float2& size);

	private:
		std::string m_debugName;
		HardAssetRef<Texture2DAsset> m_texture;
		Core::Float2 m_position; //position in screen space (top left = (0, 0) and bottom right is (width, height)
		Core::Float2 m_size; //size in pixel

		START_REFLECTION(Systems::UIBaseComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_debugName)
			ADD_FIELD(m_texture)
			ADD_FIELD(m_position)
			ADD_FIELD(m_size)
		END_REFLECTION()
	};
}

