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
	class MaterialInstanceAsset;

	class UIBaseComponent : public GameComponent
	{
	public:
		UIBaseComponent();
		~UIBaseComponent();

		const Core::Float2 GetPosition() const;
		const Core::Float2 GetScale() const;

		void SetPosition(const Core::Float2& pos);
		void SetScale(const Core::Float2& scale);

	private:
		std::string m_debugName;
		HardAssetRef<MaterialInstanceAsset> m_material;
		Core::Float2 m_position;
		Core::Float2 m_scale;

		START_REFLECTION(Systems::UIBaseComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_debugName)
			ADD_FIELD(m_material)
			ADD_FIELD(m_position)
			ADD_FIELD(m_scale)
		END_REFLECTION()
	};
}

