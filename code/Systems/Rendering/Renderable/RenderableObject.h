/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vectors.h"

#include "Systems/Rendering/RenderPass/RenderView.h"

namespace Core
{
	class Float3;
}

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
		RenderableObject();
		~RenderableObject() = default;

		void DebugSphere(const Core::Vec4f& center, float radius, const Core::Float4& color, bool wireframe);

		const Rendering::Mesh* m_pMesh;
		const Systems::MaterialInstanceAsset* m_pMaterial;
		const Systems::GameObject* m_pOwner;
		Core::Mat44f m_worldTx;

		RenderView m_view;

		bool m_debug; // Call RenderBaseShape and don't use m_pMaterial
		bool m_debugWireframe;
		Core::Float4 m_debugColor;
	};
}
