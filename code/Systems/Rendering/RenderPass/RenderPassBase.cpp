/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/RenderPass/RenderPassBase.h"

#include "Core/Math/Vectors.h"

#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Rendering/MaterialRendering.h"
#include "Systems/Rendering/Renderable/RenderableCamera.h"
#include "Systems/Rendering/Renderable/RenderableLight.h"
#include "Systems/Rendering/Renderable/RenderableObject.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"

namespace Systems
{
	uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

	RenderPassBase::RenderPassBase(uint32_t width, uint32_t height)
		: IRenderPass()
		, m_pRenderTarget(nullptr)
		, m_pShadowRenderTargets(nullptr)
		, m_shadowRenderTargetsCount(0)
		, m_pShadowMapSrvHeap(nullptr)
	{
		m_pRenderTarget = new Rendering::RenderTarget(width, height, Rendering::ResourceFormat::R8G8B8A8_UNORM_SRGB);
	}

	RenderPassBase::~RenderPassBase()
	{
		delete m_pRenderTarget;
	}

	void RenderPassBase::SetShadowMapRenderTargets(Rendering::RenderTarget** pShadowRenderTargets, uint32_t count, Rendering::DescriptorHeap* pShadowMapSrvHeap)
	{
		m_pShadowRenderTargets = pShadowRenderTargets;
		m_shadowRenderTargetsCount = count;
		m_pShadowMapSrvHeap = pShadowMapSrvHeap;
	}

	void RenderPassBase::PreRender(const RenderableScene& scene)
	{
		m_pRenderTarget->BeginScene();
	}

	void RenderPassBase::Render(const RenderableScene& scene)
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();

		const Core::Vec4f& pos = scene.m_camera.m_position;
		Core::Float3 cameraPosFloat3(pos.GetX(), pos.GetY(), pos.GetZ());
		Rendering::PerFrameCBuffer perFrameData(scene.m_camera.m_view, scene.m_camera.m_proj, cameraPosFloat3);

		//bind the shadow map
		for (uint32_t ii = 0; ii < m_shadowRenderTargetsCount; ++ii)
		{
			Rendering::Texture* pShadowMapTexture = m_pShadowRenderTargets[ii]->GetColorTexture();
			pShadowMapTexture->TransitionToShaderResource();
		}

		//for now add all lights
		Rendering::LightsArrayCBuffer lightsConstBuffer;
		Core::Mat44f lightSpace[Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT];

		uint32_t lightCount = min(scene.m_lights.GetSize(), Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT);
		for (uint32_t ii = 0; ii < lightCount; ++ii)
		{
			Rendering::LightCBuffer* pLight = lightsConstBuffer.AddLight();
			*pLight = scene.m_lights[ii].m_cbuffer;
			lightSpace[ii] = scene.m_lights[ii].m_lightSpaceTX;
		}

		Core::Mat44f viewProj = scene.m_camera.m_view * scene.m_camera.m_proj;

		//now render all renderables
		for (const Systems::RenderableObject& renderable : scene.m_objects)
		{
			if (!(renderable.m_view & Systems::RenderView::Game))
				continue;

			if (renderable.m_primitiveMesh)
			{
				renderModule.RenderBaseShape(renderable.m_pMesh, renderable.m_worldTx * viewProj, Core::Float4(1, 1, 1, 1));
			}
			else
			{
				const Systems::MaterialAsset* pMaterial = renderable.m_pMaterial->GetBaseMaterial();
				if (pMaterial && pMaterial->IsValidForRendering())
				{
					Rendering::PerObjectCBuffer perObjectData;
					perObjectData.m_world = Core::Mat44f(renderable.m_worldTx.m_matrix);
					memcpy(perObjectData.m_lightSpaceMatrix, lightSpace, sizeof(Core::Mat44f) * Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT);

					Systems::MaterialRendering::Bind(*renderable.m_pMaterial, perObjectData, perFrameData, lightsConstBuffer);

					int32_t shadowMapsRootSigIndex = pMaterial->GetShadowMapsRootSigIndex();
					if (shadowMapsRootSigIndex != -1)
					{
						ID3D12DescriptorHeap* pDescriptorHeap[] = { m_pShadowMapSrvHeap->GetHeap() };
						renderModule.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
						renderModule.GetRenderCommandList()->SetGraphicsRootDescriptorTable(shadowMapsRootSigIndex, m_pShadowMapSrvHeap->GetHeap()->GetGPUDescriptorHandleForHeapStart());
					}

					renderModule.RenderMesh(*renderable.m_pMesh);
				}
			}
		}
	}

	void RenderPassBase::PostRender(const RenderableScene& scene)
	{
		m_pRenderTarget->EndScene();
	}

	void RenderPassBase::ClearDepthBuffer()
	{
		m_pRenderTarget->ClearDepthBuffer();
	}

	Rendering::RenderTarget* RenderPassBase::GetRenderTarget()
	{
		return m_pRenderTarget;
	}
}
