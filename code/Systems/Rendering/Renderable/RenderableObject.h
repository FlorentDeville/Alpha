/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"

#include "Systems/Rendering/RenderPass/RenderView.h"

namespace Rendering
{
	class Mesh;
}

namespace Systems
{
	class GameObject;
	class MaterialInstanceAsset;

	class RenderableObject
	{
	public:
		RenderableObject()
			: m_debug(false)
		{ }

		~RenderableObject() = default;

		const Rendering::Mesh* m_pMesh;
		const Systems::MaterialInstanceAsset* m_pMaterial;
		const Systems::GameObject* m_pOwner;
		Core::Mat44f m_worldTx;

		RenderView m_view;

		bool m_debug; // Call RenderBaseShape and don't use m_pMaterial
	};
}
