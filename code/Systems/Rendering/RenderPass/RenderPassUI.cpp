/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/RenderPass/RenderPassUI.h"

#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

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

		//create the pso
		m_pRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::GAME_UI);
		
		const Rendering::ShaderMgr& shaderMgr = Rendering::ShaderMgr::Get();
		
		Rendering::PipelineStateDesc desc;
		desc.m_pPs = shaderMgr.GetShader(Rendering::EngineShaders::GAME_UI_PS);
		desc.m_pVs = shaderMgr.GetShader(Rendering::EngineShaders::GAME_UI_VS);
		desc.m_pRs = m_pRootSig;
		
		m_pPso = new Rendering::PipelineState();
		m_pPso->Init_Generic(desc);
	}

	RenderPassUI::~RenderPassUI()
	{
		delete m_pPso;
	}

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
		struct FrameParameterStruct
		{
			Core::Mat44f m_view;
			Core::Mat44f m_proj;
		};

		struct ObjectParameterStruct
		{
			Core::Float2 m_pos;
			Core::Float2 m_size;
		};

		FrameParameterStruct frameParam;
		frameParam.m_view = m_view;
		frameParam.m_proj = m_proj;

		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		renderer.BindMaterial(*m_pPso, *m_pRootSig);
		renderer.SetConstantBuffer(0, sizeof(FrameParameterStruct), &frameParam, 0);

		Rendering::PerFrameCBuffer perFrameData(m_view, m_proj, Core::Float3(0, 0, 0), scene.m_time);
		Rendering::LightsArrayCBuffer lights;

		for (const RenderableUI& obj : scene.m_uiObjects)
		{
			ObjectParameterStruct objParam;
			objParam.m_pos = obj.m_position;
			objParam.m_size = obj.m_size;
			renderer.SetConstantBuffer(1, sizeof(ObjectParameterStruct), &objParam, 0);
			
			const uint32_t textureRootSigParamIndex = 2;
			if (obj.m_pTexture)
			{
				const Rendering::Texture* pGfxTexture = obj.m_pTexture;
				ID3D12DescriptorHeap* pSrv = pGfxTexture->GetNonConstSRV();
				ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

				//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
				renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
				renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(textureRootSigParamIndex, pSrv->GetGPUDescriptorHandleForHeapStart());
			}
			else
			{
				renderer.BindNullTexture2D(textureRootSigParamIndex);
			}

			renderer.RenderInstancedQuad(1);
		}
	}

	void RenderPassUI::PostRender(const RenderableScene& scene)
	{
		m_pOutputRenderTarget->EndScene();
	}
}
