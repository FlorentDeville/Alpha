/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/RenderPass/RenderPassUI.h"

#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Systems/Rendering/MaterialRendering.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"

namespace Systems
{
	RenderPassUI::RenderPassUI(uint32_t width, uint32_t height)
		: IRenderPass()
		, m_pOutputRenderTarget(nullptr)
	{
		float halfWidth = width * 0.5f;
		float halfHeight = height * 0.5f;
		const float FAR_Z = 100;
		const float NEAR_Z = -100;

		m_view = Core::Mat44f::CreateIdentity();
		m_proj = Core::Mat44f::CreateOrthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, NEAR_Z, FAR_Z);
	}

	RenderPassUI::~RenderPassUI()
	{ }

	void RenderPassUI::SetOutput(Rendering::RenderTarget* pOutput)
	{
		m_pOutputRenderTarget = pOutput;
	}

	void RenderPassUI::PreRender(const RenderableScene& scene)
	{
		m_pOutputRenderTarget->BeginScene(false);
		m_pOutputRenderTarget->ClearDepthBuffer();
	}

	void RenderPassUI::Render(const RenderableScene& scene)
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		Rendering::PerFrameCBuffer perFrameData(m_view, m_proj, Core::Float3(0, 0, 0), scene.m_time);
		Rendering::LightsArrayCBuffer lights;

		for (const RenderableObject& obj : scene.m_uiObjects)
		{
			if ((obj.m_view & RenderView::Game) == 0)
				continue;

			Rendering::PerObjectCBuffer objCBuffer;
			objCBuffer.m_world = obj.m_worldTx;

			MaterialRendering::Bind(*obj.m_pMaterial, objCBuffer, perFrameData, lights);
			renderer.RenderMesh(*obj.m_pMesh);
		}
	}

	void RenderPassUI::PostRender(const RenderableScene& scene)
	{
		m_pOutputRenderTarget->EndScene();
	}
}
