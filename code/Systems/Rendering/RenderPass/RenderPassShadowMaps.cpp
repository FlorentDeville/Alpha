/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/RenderPass/RenderPassShadowMaps.h"

#include "Core/Math/Vec4f.h"

#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/Device.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Rendering/Renderable/RenderableLight.h"
#include "Systems/Rendering/Renderable/RenderableObject.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"

namespace Systems
{
	class ShadowMapsImpl
	{
	public:
		Rendering::RenderTarget* m_pShadowRenderTarget[Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT];
		Rendering::DescriptorHeap* m_pShadowHeapSrv; //special heap for shadow maps with contiguous srv.
		Rendering::RootSignature* m_pShadowSpotLightRootSig;
		Rendering::PipelineState* m_pShadowSpotLightPso;
		Rendering::RootSignature* m_pShadowDirLightRootSig;
		Rendering::PipelineState* m_pShadowDirLightPso;
	};

	uint8_t min(uint8_t a, uint8_t b) { return a < b ? a : b; }

	RenderPassShadowMaps::RenderPassShadowMaps()
		: IRenderPass()
	{
		m_pImpl = new ShadowMapsImpl();
		CreateRenderTargets();

		
		Rendering::RootSignatureMgr& rootSignatureMgr = Rendering::RootSignatureMgr::Get();
		Rendering::ShaderMgr& shaderMgr = Rendering::ShaderMgr::Get();
		Rendering::PipelineStateMgr& pipelineStateMgr = Rendering::PipelineStateMgr::Get();

		//shadow map spot light material
		{
			m_pImpl->m_pShadowSpotLightRootSig = rootSignatureMgr.GetRootSignature(Rendering::EngineRootSigs::SHADOWMAP_SPOTLIGHT);
			Rendering::Shader* pVS = shaderMgr.GetShader(Rendering::EngineShaders::SHADOWMAP_SPOTLIGHT_VS);
			Rendering::Shader* pPS = shaderMgr.GetShader(Rendering::EngineShaders::SHADOWMAP_SPOTLIGHT_PS);

			Rendering::PipelineStateId psoId;
			m_pImpl->m_pShadowSpotLightPso = pipelineStateMgr.CreatePipelineState(psoId);
			m_pImpl->m_pShadowSpotLightPso->Init_Generic_ShadowMap_SpotLight(m_pImpl->m_pShadowSpotLightRootSig, pVS, pPS);
		}

		//dir light shadow map material
		{
			m_pImpl->m_pShadowDirLightRootSig = rootSignatureMgr.GetRootSignature(Rendering::EngineRootSigs::SHADOWMAP_DIRLIGHT);
			Rendering::Shader* pVS = shaderMgr.GetShader(Rendering::EngineShaders::SHADOWMAP_DIRLIGHT_VS);
			Rendering::Shader* pPS = shaderMgr.GetShader(Rendering::EngineShaders::SHADOWMAP_DIRLIGHT_PS);

			Rendering::PipelineStateId psoId;
			m_pImpl->m_pShadowDirLightPso = pipelineStateMgr.CreatePipelineState(psoId);
			m_pImpl->m_pShadowDirLightPso->Init_Generic_ShadowMap_DirLight(m_pImpl->m_pShadowDirLightRootSig, pVS, pPS);
		}
	}

	RenderPassShadowMaps::~RenderPassShadowMaps()
	{
		for (int ii = 0; ii < Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT; ++ii)
			delete m_pImpl->m_pShadowRenderTarget[ii];

		m_pImpl->m_pShadowHeapSrv->Release();
		delete m_pImpl->m_pShadowHeapSrv;

		delete m_pImpl;
	}

	void RenderPassShadowMaps::PreRender(const RenderableScene& scene)
	{ }

	void RenderPassShadowMaps::Render(const RenderableScene& scene)
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();

		uint8_t lightCount = min(scene.m_lights.GetSize(), Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT);
		for (int ii = 0; ii < lightCount; ++ii)
		{
			const Systems::RenderableLight& light = scene.m_lights[ii];

			if (light.m_cbuffer.m_type == Rendering::Point)
				continue;

			if (light.m_cbuffer.m_type == Rendering::Directional)
			{
				renderModule.BindMaterial(*m_pImpl->m_pShadowDirLightPso, *m_pImpl->m_pShadowDirLightRootSig);

				renderModule.SetConstantBuffer(1, sizeof(Core::Mat44f), &light.m_lightSpaceTX, 0);
			}
			else if (light.m_cbuffer.m_type == Rendering::Spot)
			{
				renderModule.BindMaterial(*m_pImpl->m_pShadowSpotLightPso, *m_pImpl->m_pShadowSpotLightRootSig);

				struct PerObject
				{
					Core::Mat44f m_lightSpace;
					Core::Float3 m_lightPos;
				};

				PerObject cbuffer;
				cbuffer.m_lightSpace = light.m_lightSpaceTX;
				cbuffer.m_lightPos = light.m_cbuffer.m_position;

				//bind the light space matrix
				renderModule.SetConstantBuffer(1, sizeof(PerObject), &cbuffer, 0);
			}

			m_pImpl->m_pShadowRenderTarget[ii]->BeginScene();

			//loop through renderable
			for (const Systems::RenderableObject& renderable : scene.m_objects)
			{
				if (!(renderable.m_view & Systems::RenderView::ShadowMap))
					continue;

				renderModule.SetConstantBuffer(0, sizeof(Core::Mat44f), &renderable.m_worldTx, 0);
				renderModule.RenderMesh(*renderable.m_pMesh);
			}

			m_pImpl->m_pShadowRenderTarget[ii]->EndScene();
		}
	}

	void RenderPassShadowMaps::PostRender(const RenderableScene& scene)
	{ }

	uint32_t RenderPassShadowMaps::GetRenderTargetsCount() const
	{
		return Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT;
	}

	Rendering::RenderTarget** RenderPassShadowMaps::GetRenderTargets()
	{
		return m_pImpl->m_pShadowRenderTarget;
	}

	Rendering::DescriptorHeap* RenderPassShadowMaps::GetSrvHeap()
	{
		return m_pImpl->m_pShadowHeapSrv;
	}

	void RenderPassShadowMaps::CreateRenderTargets()
	{
		Rendering::Device* pDevice = Rendering::RenderModule::Get().GetDevice();
		ID3D12Device2* pDx12Device = pDevice->GetDx12Device();

		m_pImpl->m_pShadowHeapSrv = new Rendering::DescriptorHeap();
		m_pImpl->m_pShadowHeapSrv->Init(pDevice, Rendering::DescriptorHeapFlag::SHADER_VISIBLE, Rendering::DescriptorHeapType::CBV_SRV_UAV, Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT);

		DXGI_FORMAT dxFormat = DXGI_FORMAT_R32_FLOAT;
		Rendering::BufferFormat format = Rendering::BufferFormat::R32_FLOAT;
		for (int ii = 0; ii < Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT; ++ii)
		{
			m_pImpl->m_pShadowRenderTarget[ii] = new Rendering::RenderTarget(1024, 1024, format, Core::Vec4f(1, 1, 1, 1));

			//Create the srv descriptor
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = dxFormat;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Texture2D.MipLevels = 1;

				D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pImpl->m_pShadowHeapSrv->GetNewHandle();
				pDx12Device->CreateShaderResourceView(m_pImpl->m_pShadowRenderTarget[ii]->GetColorTexture()->GetResource(), &srvDesc, handle);
			}
		}
	}
}
