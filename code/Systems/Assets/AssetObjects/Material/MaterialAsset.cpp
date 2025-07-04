/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"

#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/Shaders/Shader.h"

namespace Systems
{
	MaterialAsset_v1::MaterialAsset_v1()
		: m_sourceFilePS()
		, m_sourceFileVS()
		, m_pVs(nullptr)
		, m_pPs(nullptr)
		, m_pRs(nullptr)
		, m_pPipelineState(nullptr)
		, m_hasPerFrameParameters(false)
		, m_hasPerObjectParameters(false)
		, m_hasLightsParameters(false)
		, m_perFrameCBufferRootSignatureIndex(0)
		, m_perObjectCBufferRootSignatureIndex(0)
		, m_perMaterialCBufferRootSignatureIndex(0)
		, m_lightsCBufferRootSignatureIndex(0)
	{ }

	MaterialAsset_v1::~MaterialAsset_v1()
	{
		delete m_pVs;
		delete m_pPs;
		delete m_pRs;
		delete m_pPipelineState;
	}

	const std::string& MaterialAsset_v1::GetSourceFilePs() const
	{
		return m_sourceFilePS;
	}

	const std::string& MaterialAsset_v1::GetSourceFileVs() const
	{
		return m_sourceFileVS;
	}

	const Rendering::RootSignature* MaterialAsset_v1::GetRootSignature() const
	{
		return m_pRs;
	}

	const Rendering::PipelineState* MaterialAsset_v1::GetPipelineState() const
	{
		return m_pPipelineState;
	}

	Core::Array<char>& MaterialAsset_v1::GetPsBlob()
	{
		return m_psBlob;
	}

	Core::Array<char>& MaterialAsset_v1::GetVsBlob()
	{
		return m_vsBlob;
	}

	Core::Array<char>& MaterialAsset_v1::GetRsBlob()
	{
		return m_rsBlob;
	}

	Core::Array<MaterialParameterDescription>& MaterialAsset_v1::GetMaterialParameterDescription()
	{
		return m_perMaterialParameters;
	}

	void MaterialAsset_v1::PostLoad()
	{
		UpdateRenderingObjects();
	}

	void MaterialAsset_v1::UpdateRenderingObjects()
	{
		bool validVertexShader = false;
		if (m_vsBlob.GetSize() != 0)
		{
			delete m_pVs;
			m_pVs = new Rendering::Shader();
			bool res = m_pVs->LoadFromMemory(m_vsBlob);
			if (!res)
				return;

			validVertexShader = true;
		}

		bool validPixelShader = false;
		if (m_psBlob.GetSize() != 0)
		{
			delete m_pPs;
			m_pPs = new Rendering::Shader();
			bool res = m_pPs->LoadFromMemory(m_psBlob);
			if (!res)
				return;

			validPixelShader = true;
		}

		bool validRootSignature = false;
		if (m_rsBlob.GetSize() != 0)
		{
			delete m_pRs;
			m_pRs = new Rendering::RootSignature();
			bool res = m_pRs->LoadFromMemory(m_rsBlob);
			if (!res)
				return;

			validRootSignature = true;
		}

		bool createPipelineState = validPixelShader && validVertexShader && validRootSignature;
		if (createPipelineState)
		{
			if (m_pPipelineState)
				delete m_pPipelineState;

			m_pPipelineState = new Rendering::PipelineState();
			m_pPipelineState->Init_Generic(*m_pRs, *m_pVs, *m_pPs);
		}
	}

	bool MaterialAsset_v1::IsValidForRendering() const
	{
		return m_pPipelineState && m_pRs;
	}

	void MaterialAsset_v1::SetHasPerObjectParameters(bool enable)
	{
		m_hasPerObjectParameters = enable;
	}

	void MaterialAsset_v1::SetHasPerFrameParameters(bool enable)
	{
		m_hasPerFrameParameters = enable;
	}

	void MaterialAsset_v1::SetHasLightsParameters(bool enable)
	{
		m_hasLightsParameters = enable;
	}

	bool MaterialAsset_v1::HasPerObjectParameters() const
	{
		return m_hasPerObjectParameters;
	}

	bool MaterialAsset_v1::HasPerFrameParameters() const
	{
		return m_hasPerFrameParameters;
	}

	bool MaterialAsset_v1::HasLightsParameters() const
	{
		return m_hasLightsParameters;
	}

	void MaterialAsset_v1::SetPerFrameRootSignatureParameterIndex(uint32_t index)
	{
		m_perFrameCBufferRootSignatureIndex = index;
	}

	void MaterialAsset_v1::SetPerObjectRootSignatureParameterIndex(uint32_t index)
	{
		m_perObjectCBufferRootSignatureIndex = index;
	}

	void MaterialAsset_v1::SetPerMaterialRootSignatureParameterIndex(uint32_t index)
	{
		m_perMaterialCBufferRootSignatureIndex = index;
	}

	void MaterialAsset_v1::SetLightsRootSignatureParameterIndex(uint32_t index)
	{
		m_lightsCBufferRootSignatureIndex = index;
	}

	uint32_t MaterialAsset_v1::GetPerFrameRootSignatureParameterIndex() const
	{
		return m_perFrameCBufferRootSignatureIndex;
	}

	uint32_t MaterialAsset_v1::GetPerObjectRootSignatureParameterIndex() const
	{
		return m_perObjectCBufferRootSignatureIndex;
	}

	uint32_t MaterialAsset_v1::GetPerMaterialRootSignatureParameterIndex() const
	{
		return m_perMaterialCBufferRootSignatureIndex;
	}

	uint32_t MaterialAsset_v1::GetLightsRootSignatureParameterIndex() const
	{
		return m_lightsCBufferRootSignatureIndex;
	}
}
