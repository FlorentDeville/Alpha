/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Rendering/MaterialRendering.h"

#include "Rendering/ConstantBuffer/LinearConstantBufferPool.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/RenderModule.h"

namespace Systems
{
	void MaterialRendering::Bind(const MaterialAsset& material, const Rendering::PerObjectCBuffer& perObjectCBuffer, 
		const Rendering::PerFrameCBuffer& perFrameCBuffer)
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		renderer.BindMaterial(*material.GetPipelineState(), *material.GetRootSignature());

		if (material.HasPerObjectParameters())
		{
			Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
			int poolIndex = pCBufferPool->GetFreeConstantBufferIndex();
			pCBufferPool->Copy(poolIndex, &perObjectCBuffer, sizeof(Rendering::PerObjectCBuffer));
			renderer.BindCBuffer(material.GetPerObjectRootSignatureParameterIndex(), poolIndex);
		}

		if (material.HasPerFrameParameters())
		{
			Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
			int poolIndex = pCBufferPool->GetFreeConstantBufferIndex();
			pCBufferPool->Copy(poolIndex, &perFrameCBuffer, sizeof(Rendering::PerFrameCBuffer));
			renderer.BindCBuffer(material.GetPerObjectRootSignatureParameterIndex(), poolIndex);
		}

		const Core::Array<Systems::MaterialParameterDescription>& perMaterialParam = material.GetMaterialParameterDescription();
		if (perMaterialParam.GetSize() > 0)
		{
			Rendering::LinearConstantBufferPool* pCBufferPool = renderer.GetLinearCBufferPool();
			int poolIndex = pCBufferPool->GetFreeConstantBufferIndex();

			for (uint32_t ii = 0; ii < perMaterialParam.GetSize(); ++ii)
			{
				const Systems::MaterialParameterDescription& pParam = perMaterialParam[ii];

				if (pParam.m_value.GetSize() == 0)
					continue;

				pCBufferPool->Copy(poolIndex, pParam.m_offset, pParam.m_value.GetData(), pParam.m_size);
			}
			renderer.BindCBuffer(material.GetPerMaterialRootSignatureParameterIndex(), poolIndex);
		}
	}
}
