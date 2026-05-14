/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/Renderable/RenderableObject.h"

#include "Rendering/RenderModule.h"

namespace Systems
{
	RenderableObject::RenderableObject()
		: m_debug(false)
		, m_debugWireframe(false)
		, m_debugColor(1, 1, 1, 1)
	{ }

	void RenderableObject::DebugSphere(const Core::Vec4f& center, float radius, const Core::Float4& color, bool wireframe)
	{
		m_pMesh = Rendering::RenderModule::Get().m_pSphereMesh;
		m_pMaterial = nullptr;
		
		Core::Mat44f translation = Core::Mat44f::CreateTranslationMatrix(center);
		Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(radius);
		m_worldTx = scale * translation;
		
		m_debug = true;
		m_debugWireframe = wireframe;
		m_debugColor = color;
	}
}
