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
		, m_pInputTexture(nullptr)
		, m_pOutputRenderTarget(nullptr)
		, m_bloomFilterRadius(0.005f)
	{
		//emissive filter
		m_pEmissive = new Rendering::RenderTarget(width, height, Rendering::BufferFormat::R16G16B16A16_FLOAT, Core::Vec4f());

		m_pEmissiveFilterRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::BLOOM_EMISSIVE_FILTER);
		Rendering::Shader* pBloomEmissiveFilterVs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_EMISSIVE_FILTER_VS);
		Rendering::Shader* pBloomEmissiveFilterPs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_EMISSIVE_FILTER_PS);
		Rendering::PipelineStateDesc bloomEmissiveFilterPsoDesc;
		bloomEmissiveFilterPsoDesc.m_pRs = m_pEmissiveFilterRootSig;
		bloomEmissiveFilterPsoDesc.m_pVs = pBloomEmissiveFilterVs;
		bloomEmissiveFilterPsoDesc.m_pPs = pBloomEmissiveFilterPs;
		bloomEmissiveFilterPsoDesc.m_depthStencilDesc.m_depthFunction = Rendering::DepthComparisonMode::ALWAYS;
		m_pEmissiveFilterPso = new Rendering::PipelineState();
		m_pEmissiveFilterPso->Init_Generic(bloomEmissiveFilterPsoDesc);

		//mip chain
		m_ppMipRenderTarget = new Rendering::RenderTarget* [mipCount];
		m_mipSizes = new Core::Float2[mipCount];

		uint32_t mipWidth = width;
		uint32_t mipHeight = height;

		for (uint32_t ii = 0; ii < mipCount; ++ii)
		{
			mipWidth = mipWidth / 2;
			mipHeight = mipHeight / 2;

			m_mipSizes[ii].x = static_cast<float>(mipWidth);
			m_mipSizes[ii].y = static_cast<float>(mipHeight);

			m_ppMipRenderTarget[ii] = new Rendering::RenderTarget(mipWidth, mipHeight, Rendering::BufferFormat::R16G16B16A16_FLOAT, Core::Vec4f());
		}

		//downsampling
		m_pDownsamplingRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::BLOOM_DOWNSAMPLE);
		Rendering::Shader* pDownsamplingVS = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_DOWNSAMPLE_VS);
		Rendering::Shader* pDownsamplingPS = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_DOWNSAMPLE_PS);
		Rendering::PipelineStateDesc downsamplingPsoDesc;
		downsamplingPsoDesc.m_pRs = m_pDownsamplingRootSig;
		downsamplingPsoDesc.m_pVs = pDownsamplingVS;
		downsamplingPsoDesc.m_pPs = pDownsamplingPS;
		downsamplingPsoDesc.m_depthStencilDesc.m_depthFunction = Rendering::DepthComparisonMode::ALWAYS;
		m_pDownsamplingPso = new Rendering::PipelineState();
		m_pDownsamplingPso->Init_Generic(downsamplingPsoDesc);

		//upsampling
		m_pUpsamplingRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::BLOOM_UPSAMPLE);
		Rendering::Shader* pUpsamplingVS = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_UPSAMPLE_VS);
		Rendering::Shader* pUpsamplingPS = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::BLOOM_UPSAMPLE_PS);
		Rendering::PipelineStateDesc upsamplingPsoDesc;
		upsamplingPsoDesc.m_pRs = m_pUpsamplingRootSig;
		upsamplingPsoDesc.m_pVs = pUpsamplingVS;
		upsamplingPsoDesc.m_pPs = pUpsamplingPS;
		upsamplingPsoDesc.m_depthStencilDesc.m_depthFunction = Rendering::DepthComparisonMode::ALWAYS;
		upsamplingPsoDesc.m_blendDesc.m_blendEnabled = true;
		upsamplingPsoDesc.m_blendDesc.m_srcBlend = Rendering::BlendFactor::ONE;
		upsamplingPsoDesc.m_blendDesc.m_dstBlend = Rendering::BlendFactor::ONE;
		upsamplingPsoDesc.m_blendDesc.m_blendOperation = Rendering::BlendOperation::ADD;
		upsamplingPsoDesc.m_blendDesc.m_srcBlendAlpha = Rendering::BlendFactor::ZERO;
		upsamplingPsoDesc.m_blendDesc.m_dstBlendAlpha = Rendering::BlendFactor::ONE;
		upsamplingPsoDesc.m_blendDesc.m_blendOperationAlpha = Rendering::BlendOperation::ADD;
		m_pUpsamplingPso = new Rendering::PipelineState();
		m_pUpsamplingPso->Init_Generic(upsamplingPsoDesc);

		//combining
		Rendering::PipelineStateDesc combiningPsoDesc;
		combiningPsoDesc.m_pRs = m_pUpsamplingRootSig;
		combiningPsoDesc.m_pVs = pUpsamplingVS;
		combiningPsoDesc.m_pPs = pUpsamplingPS;
		combiningPsoDesc.m_depthStencilDesc.m_depthFunction = Rendering::DepthComparisonMode::ALWAYS;
		combiningPsoDesc.m_blendDesc.m_blendEnabled = true;
		combiningPsoDesc.m_blendDesc.m_srcBlend = Rendering::BlendFactor::FACTOR;
		combiningPsoDesc.m_blendDesc.m_dstBlend = Rendering::BlendFactor::INV_FACTOR;
		combiningPsoDesc.m_blendDesc.m_blendOperation = Rendering::BlendOperation::ADD;
		combiningPsoDesc.m_blendDesc.m_srcBlendAlpha = Rendering::BlendFactor::ZERO;
		combiningPsoDesc.m_blendDesc.m_dstBlendAlpha = Rendering::BlendFactor::ONE;
		combiningPsoDesc.m_blendDesc.m_blendOperationAlpha = Rendering::BlendOperation::ADD;
		m_pCombiningPso = new Rendering::PipelineState();
		m_pCombiningPso->Init_Generic(combiningPsoDesc);
	}

	RenderPassBloom::~RenderPassBloom()
	{
		for (uint32_t ii = 0; ii < m_mipCount; ++ii)
			delete m_ppMipRenderTarget[ii];

		delete[] m_ppMipRenderTarget;
		delete[] m_mipSizes;

		delete m_pEmissive;
		delete m_pEmissiveFilterPso;

		delete m_pDownsamplingPso;
		delete m_pUpsamplingPso;
		delete m_pCombiningPso;
	}

	void RenderPassBloom::SetInput(Rendering::Texture* pInput)
	{
		m_pInputTexture = pInput;
	}

	void RenderPassBloom::SetOutput(Rendering::RenderTarget* pOutput)
	{
		m_pOutputRenderTarget = pOutput;
	}

	void RenderPassBloom::PreRender(const RenderableScene& scene)
	{
		m_pInputTexture->TransitionToShaderResource();
	}

	void RenderPassBloom::Render(const RenderableScene& scene)
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		//extract emissive.
		//todo : use directly the color buffer since I don't filte anything.
		m_pEmissive->BeginScene();
		renderer.BindMaterial(*m_pEmissiveFilterPso, *m_pEmissiveFilterRootSig);

		{
			ID3D12DescriptorHeap* pSrv = m_pInputTexture->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

			//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
			renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(0, pSrv->GetGPUDescriptorHandleForHeapStart());
		}

		renderer.RenderNoBufferTriangle();
		m_pEmissive->EndScene();

		//downsample

		//set the pso, rootsig for downsampling
		renderer.BindMaterial(*m_pDownsamplingPso, *m_pDownsamplingRootSig);
		Rendering::Texture* pSource = m_pEmissive->GetColorTexture();
		for (uint32_t ii = 0; ii < m_mipCount; ++ii)
		{
			pSource->TransitionToShaderResource();
			Rendering::RenderTarget* pCurrentTarget = m_ppMipRenderTarget[ii];
			pCurrentTarget->BeginScene();

			renderer.SetConstantBuffer(0, sizeof(Core::Float2), &m_mipSizes[ii], 0);
			{
				ID3D12DescriptorHeap* pSrv = pSource->GetSRV();
				ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

				//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
				renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
				renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());
			}

			renderer.RenderNoBufferTriangle();
			pCurrentTarget->EndScene();

			pSource = pCurrentTarget->GetColorTexture();
		}

		//now upsample the mip chain

		//set the pso and rootsig for upsampling
		renderer.BindMaterial(*m_pUpsamplingPso, *m_pUpsamplingRootSig);
		renderer.SetConstantBuffer(0, sizeof(float), &m_bloomFilterRadius, 0);
		for (int32_t ii = m_mipCount - 2; ii >= 0; --ii)
		{
			Rendering::RenderTarget* pRenderTarget = m_ppMipRenderTarget[ii];
			pRenderTarget->BeginScene(false);

			Rendering::Texture* pSource = m_ppMipRenderTarget[ii + 1]->GetColorTexture();
			pSource->TransitionToShaderResource();
				
			{
				ID3D12DescriptorHeap* pSrv = pSource->GetSRV();
				ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

				//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
				renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
				renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());
			}

			//render null triangle
			renderer.RenderNoBufferTriangle();

			pRenderTarget->EndScene();
		}

		//composition : do a last upsampling between the 1st mip and the output
		renderer.BindMaterial(*m_pCombiningPso, *m_pUpsamplingRootSig);
		{
			m_pOutputRenderTarget->BeginScene(false);
			const float blend = 0.04f;
			renderer.SetBlendFactor(Core::Float4(blend, blend, blend, 1));

			Rendering::Texture* pSource = m_ppMipRenderTarget[0]->GetColorTexture();

			{
				ID3D12DescriptorHeap* pSrv = pSource->GetSRV();
				ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

				//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
				renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
				renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());
			}

			renderer.RenderNoBufferTriangle();
			m_pOutputRenderTarget->EndScene();
		}
	}

	void RenderPassBloom::PostRender(const RenderableScene& scene)
	{ }
}
