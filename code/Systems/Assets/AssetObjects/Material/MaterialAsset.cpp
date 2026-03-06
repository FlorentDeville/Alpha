/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset_v2.h"

#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/Shaders/Shader.h"

namespace Systems
{
	MaterialAsset::MaterialAsset()
		: AssetObject()
		, m_sourceFilePS()
		, m_sourceFileVS()
		, m_pVs(nullptr)
		, m_pPs(nullptr)
		, m_pRs(nullptr)
		, m_pPipelineState(nullptr)
		, m_isDebug(false)
		, m_cullMode(Rendering::CullMode::Back)
		, m_depthFunction(Rendering::DepthComparisonMode::Less)
		, m_shadowMapsRootSigIndex(-1)
	{ }

	MaterialAsset::~MaterialAsset()
	{
		delete m_pVs;
		delete m_pPs;
		delete m_pRs;
		delete m_pPipelineState;
	}

	const std::string& MaterialAsset::GetSourceFilePs() const
	{
		return m_sourceFilePS;
	}

	const std::string& MaterialAsset::GetSourceFileVs() const
	{
		return m_sourceFileVS;
	}

	void MaterialAsset::SetSourceFilePs(const std::string& psFilename)
	{
		m_sourceFilePS = psFilename;
	}

	void MaterialAsset::SetSourceFileVs(const std::string& vsFilename)
	{
		m_sourceFileVS = vsFilename;
	}

	const Rendering::RootSignature* MaterialAsset::GetRootSignature() const
	{
		return m_pRs;
	}

	const Rendering::PipelineState* MaterialAsset::GetPipelineState() const
	{
		return m_pPipelineState;
	}

	Core::Blob& MaterialAsset::GetPsBlob()
	{
		return m_psBlob;
	}

	Core::Blob& MaterialAsset::GetVsBlob()
	{
		return m_vsBlob;
	}

	Core::Blob& MaterialAsset::GetRsBlob()
	{
		return m_rsBlob;
	}

	Core::Array<MaterialParameterDescription>& MaterialAsset::GetMaterialParameterDescription()
	{
		return m_perMaterialParameters;
	}

	const Core::Array<MaterialParameterDescription>& MaterialAsset::GetMaterialParameterDescription() const
	{
		return m_perMaterialParameters;
	}

	Core::Array<MaterialCBufferBindingInfo>& MaterialAsset::GetBindingInfoArray()
	{
		return m_bindingInfoArray;
	}

	const Core::Array<MaterialCBufferBindingInfo>& MaterialAsset::GetBindingInfoArray() const
	{
		return m_bindingInfoArray;
	}

	Core::Array<TextureBindingInfo>& MaterialAsset::GetTexturesBindingInfo()
	{
		return m_texturesBindingInfo;
	}

	const Core::Array<TextureBindingInfo>& MaterialAsset::GetTexturesBindingInfo() const
	{
		return m_texturesBindingInfo;
	}

	void MaterialAsset::PostLoad()
	{
		UpdateRenderingObjects();
	}

	void MaterialAsset::UpdateRenderingObjects()
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

			Rendering::PipelineStateDesc desc;
			desc.m_pRs = m_pRs;
			desc.m_pVs = m_pVs;
			desc.m_pPs = m_pPs;
			desc.m_cullMode = m_cullMode;
			desc.m_depthFunction = m_depthFunction;

			m_pPipelineState = new Rendering::PipelineState();
			m_pPipelineState->Init_Generic(desc);
		}
	}

	bool MaterialAsset::IsValidForRendering() const
	{
		return m_pPipelineState && m_pRs;
	}

	void MaterialAsset::SetIsDebug(bool debug)
	{
		m_isDebug = debug;
	}

	bool MaterialAsset::IsDebug() const
	{
		return m_isDebug;
	}

	void MaterialAsset::SetShadowMapsRootSigIndex(int32_t index)
	{
		m_shadowMapsRootSigIndex = index;
	}

	int32_t MaterialAsset::GetShadowMapsRootSigIndex() const
	{
		return m_shadowMapsRootSigIndex;
	}

	const std::string& MaterialAsset::GetAssetTypeName()
	{
		static std::string name = "Material";
		return name;
	}

	Core::Sid MaterialAsset::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	void MaterialAsset::Upgrade(const MaterialAsset_v2* pV2)
	{
		m_sourceFilePS = pV2->m_sourceFilePS;
		m_sourceFileVS = pV2->m_sourceFileVS;
		m_psBlob = pV2->m_psBlob;
		m_vsBlob = pV2->m_vsBlob;
		m_rsBlob = pV2->m_rsBlob;
		m_bindingInfoArray = pV2->m_bindingInfoArray;
		m_perMaterialParameters = pV2->m_perMaterialParameters;
		m_texturesBindingInfo = pV2->m_texturesBindingInfo;
		for (TextureBindingInfo& info : m_texturesBindingInfo)
		{
			info.m_texture.Resolve();
		}
		m_cullMode = pV2->m_cullMode;
		m_depthFunction = pV2->m_depthFunction;
		m_shadowMapsRootSigIndex = pV2->m_shadowMapsRootSigIndex;

		PostLoad();
	}
}
