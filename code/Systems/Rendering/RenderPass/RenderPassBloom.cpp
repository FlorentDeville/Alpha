/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/RenderPass/RenderPassBloom.h"

#include "Core/Math/Vec4f.h"

#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

namespace Systems
{
	RenderPassBloom::RenderPassBloom(uint32_t width, uint32_t height, uint32_t mipCount)
		: IRenderPass()
		, m_mipCount(mipCount)
		, m_pFrameBuffer(nullptr)
	{
		m_pEmissive = new Rendering::RenderTarget(width, height, Rendering::ResourceFormat::R16G16B16A16_FLOAT, Core::Vec4f());

		m_pEmissiveFilterRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::BLOOM_EMISSIVE_FILTER);
		Rendering::Shader* pBloomEmissiveFilterVs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_EMISSIVE_FILTER_VS);
		Rendering::Shader* pBloomEmissiveFilterPs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_EMISSIVE_FILTER_PS);
		Rendering::PipelineStateDesc bloomEmissiveFilterPsoDesc;
		bloomEmissiveFilterPsoDesc.m_pRs = m_pEmissiveFilterRootSig;
		bloomEmissiveFilterPsoDesc.m_pVs = pBloomEmissiveFilterVs;
		bloomEmissiveFilterPsoDesc.m_pPs = pBloomEmissiveFilterPs;
		bloomEmissiveFilterPsoDesc.m_depthFunction = Rendering::DepthComparisonMode::Always;
		m_pEmissiveFilterPso = new Rendering::PipelineState();
		m_pEmissiveFilterPso->Init_Generic(bloomEmissiveFilterPsoDesc);

		m_ppRenderTarget = new Rendering::RenderTarget* [mipCount];

		uint32_t mipWidth = width;
		uint32_t mipHeight = height;

		for (uint32_t ii = 0; ii < mipCount; ++ii)
		{
			mipWidth = mipWidth / 2;
			mipHeight = mipHeight / 2;

			m_ppRenderTarget[ii] = new Rendering::RenderTarget(mipWidth, mipHeight, Rendering::ResourceFormat::R16G16B16A16_FLOAT, Core::Vec4f());
		}
	}

	RenderPassBloom::~RenderPassBloom()
	{
		for (uint32_t ii = 0; ii < m_mipCount; ++ii)
			delete m_ppRenderTarget[ii];

		delete m_ppRenderTarget;

		delete m_pEmissive;
		delete m_pEmissiveFilterPso;
	}

	void RenderPassBloom::SetFrameBuffer(Rendering::Texture* pFrameBuffer)
	{
		m_pFrameBuffer = pFrameBuffer;
	}

	void RenderPassBloom::PreRender(const RenderableScene& scene)
	{
		m_pFrameBuffer->TransitionToShaderResource();
	}

	void RenderPassBloom::Render(const RenderableScene& scene)
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		//extract emissive
		m_pEmissive->BeginScene();
		renderer.BindMaterial(*m_pEmissiveFilterPso, *m_pEmissiveFilterRootSig);

		{
			ID3D12DescriptorHeap* pSrv = m_pFrameBuffer->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

			//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
			renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(0, pSrv->GetGPUDescriptorHandleForHeapStart());
		}

		renderer.RenderNoBufferTriangle();
		m_pEmissive->EndScene();

		//downsample

		//set the pso, rootsig for downsampling

		for (uint32_t ii = 0; ii < m_mipCount; ++ii)
		{
			// bind the rtv
			// bind the texture
			// render null triangle	
		}

		//now upsample

		//set the pso and rootsig for upsampling
		for (int32_t ii = m_mipCount - 1; ii >= 0; --ii)
		{
			//bind the rtv
			//bind the texture
			//render null triangle
		}

		//composition
		//add frame buffer and mip0
	}

	void RenderPassBloom::PostRender(const RenderableScene& scene)
	{ }
}
