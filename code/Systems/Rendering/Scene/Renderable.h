/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"

#include "Systems/Rendering/RenderPass/RenderPassId.h"

namespace Rendering
{
	class Mesh;
}

namespace Systems
{
	class GameObject;
	class MaterialInstanceAsset;

	class Renderable
	{
	public:
		const Rendering::Mesh* m_pMesh;
		const Systems::MaterialInstanceAsset* m_pMaterial;
		const Systems::GameObject* m_pOwner;
		Core::Mat44f m_worldTx;

		RenderPassId m_view;
		bool m_primitiveMesh; // Call RenderBaseShape and don't use m_pMaterial
	};
}
