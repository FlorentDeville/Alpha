/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Rendering/MaterialRendering.h"

#include "Rendering/ConstantBuffer/LinearConstantBufferPool.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/RenderModule.h"
#include "Rendering/Texture/Texture.h"

namespace Systems
{
	void MaterialRendering::Bind(const MaterialAsset& material, const Rendering::PerObjectCBuffer& perObjectCBuffer, 
		const Rendering::PerFrameCBuffer& perFrameCBuffer,
		const Rendering::LightsArrayCBuffer& lights)
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		renderer.BindMaterial(*material.GetPipelineState(), *material.GetRootSignature());

		int perObjectPoolIndex = -1;
		int perFramePoolIndex = -1;
		int lightsPoolIndex = -1;
		int perMaterialPoolIndex = -1;

		const Core::Array<MaterialCBufferBindingInfo> bindingInfoArray = material.GetBindingInfoArray();
		for (const MaterialCBufferBindingInfo& bindingInfo : bindingInfoArray)
		{
			switch (bindingInfo.m_type)
			{
			case CBufferType::PerObject:
			{
				if (perObjectPoolIndex == -1)
				{
					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					perObjectPoolIndex = pCBufferPool->GetFreeConstantBufferIndex();
					pCBufferPool->Copy(perObjectPoolIndex, &perObjectCBuffer, sizeof(Rendering::PerObjectCBuffer));
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, perObjectPoolIndex);
			}
			break;

			case CBufferType::PerFrame:
			{
				if (perFramePoolIndex == -1)
				{
					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					perFramePoolIndex = pCBufferPool->GetFreeConstantBufferIndex();
					pCBufferPool->Copy(perFramePoolIndex, &perFrameCBuffer, sizeof(Rendering::PerFrameCBuffer));
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, perFramePoolIndex);
			}
			break;

			case CBufferType::Lights:
			{
				if (lightsPoolIndex == -1)
				{
					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					lightsPoolIndex = pCBufferPool->GetFreeConstantBufferIndex();
					pCBufferPool->Copy(lightsPoolIndex, &lights, sizeof(Rendering::LightsArrayCBuffer));
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, lightsPoolIndex);
			}
			break;

			case CBufferType::PerMaterial:
			{
				if (perMaterialPoolIndex == -1)
				{
					const Core::Array<Systems::MaterialParameterDescription>& perMaterialParam = material.GetMaterialParameterDescription();

					//if there is a cbuffer in the root signature but no variable in it, it's weird, it shouldn't be there!!
					assert(perMaterialParam.GetSize() > 0);
					
					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					perMaterialPoolIndex = pCBufferPool->GetFreeConstantBufferIndex();

					for(const Systems::MaterialParameterDescription& pParam : perMaterialParam)
					{
						if (pParam.m_value.GetSize() == 0)
							continue;

						pCBufferPool->Copy(perMaterialPoolIndex, pParam.m_offset, pParam.m_value.GetData(), pParam.m_size);
					}
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, perMaterialPoolIndex);
			}
			break;

			default:
				assert(false); //unknown type of cbuffer
				break;
			}
		}

		const Core::Array<TextureBindingInfo>& textureBindingInfo = material.GetTexturesBindingInfo();
		for (TextureBindingInfo& bindingInfo : textureBindingInfo)
		{
			Systems::TextureAsset* pTexture = bindingInfo.m_texture.GetPtr();
			if (!pTexture)
				continue;

			Rendering::Texture* pGfxTexture = pTexture->GetTexture();
			ID3D12DescriptorHeap* pSrv = pGfxTexture->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

			//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
			renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(bindingInfo.m_sigRootIndex, pSrv->GetGPUDescriptorHandleForHeapStart());
		}
	}

	void MaterialRendering::Bind(const MaterialInstanceAsset& materialInstance, const Rendering::PerObjectCBuffer& perObjectCBuffer,
		const Rendering::PerFrameCBuffer& perFrameCBuffer,
		const Rendering::LightsArrayCBuffer& lights)
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		const MaterialAsset* pBaseMaterial = materialInstance.GetBaseMaterial();

		renderer.BindMaterial(*pBaseMaterial->GetPipelineState(), *pBaseMaterial->GetRootSignature());

		int perObjectPoolIndex = -1;
		int perFramePoolIndex = -1;
		int lightsPoolIndex = -1;
		int perMaterialPoolIndex = -1;

		const Core::Array<MaterialCBufferBindingInfo> bindingInfoArray = pBaseMaterial->GetBindingInfoArray();
		for (const MaterialCBufferBindingInfo& bindingInfo : bindingInfoArray)
		{
			switch (bindingInfo.m_type)
			{
			case CBufferType::PerObject:
			{
				if (perObjectPoolIndex == -1)
				{
					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					perObjectPoolIndex = pCBufferPool->GetFreeConstantBufferIndex();
					pCBufferPool->Copy(perObjectPoolIndex, &perObjectCBuffer, sizeof(Rendering::PerObjectCBuffer));
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, perObjectPoolIndex);
			}
			break;

			case CBufferType::PerFrame:
			{
				if (perFramePoolIndex == -1)
				{
					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					perFramePoolIndex = pCBufferPool->GetFreeConstantBufferIndex();
					pCBufferPool->Copy(perFramePoolIndex, &perFrameCBuffer, sizeof(Rendering::PerFrameCBuffer));
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, perFramePoolIndex);
			}
			break;

			case CBufferType::Lights:
			{
				if (lightsPoolIndex == -1)
				{
					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					lightsPoolIndex = pCBufferPool->GetFreeConstantBufferIndex();
					pCBufferPool->Copy(lightsPoolIndex, &lights, sizeof(Rendering::LightsArrayCBuffer));
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, lightsPoolIndex);
			}
			break;

			case CBufferType::PerMaterial:
			{
				if (perMaterialPoolIndex == -1)
				{
					const Core::Array<Systems::MaterialParameterDescription>& perMaterialParam = materialInstance.GetMaterialParameterDescription();

					//if there is a cbuffer in the root signature but no variable in it, it's weird, it shouldn't be there!!
					assert(perMaterialParam.GetSize() > 0);

					Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
					perMaterialPoolIndex = pCBufferPool->GetFreeConstantBufferIndex();

					for (const Systems::MaterialParameterDescription& pParam : perMaterialParam)
					{
						if (pParam.m_value.GetSize() == 0)
							continue;

						pCBufferPool->Copy(perMaterialPoolIndex, pParam.m_offset, pParam.m_value.GetData(), pParam.m_size);
					}
				}

				renderer.BindCBuffer(bindingInfo.m_sigRootParamIndex, perMaterialPoolIndex);
			}
			break;

			default:
				assert(false); //unknown type of cbuffer
				break;
			}
		}
	}
}
