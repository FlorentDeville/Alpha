/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"

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
		, m_cullMode(Rendering::CullMode::BACK)
		, m_depthFunction(Rendering::DepthComparisonMode::LESS)
		, m_blendMode(BlendMode::BM_OPAQUE)
		, m_shadowMapsRootSigIndex(-1)
		, m_parametersSchemaHash()
	{ }

	MaterialAsset::~MaterialAsset()
	{
		delete m_pVs;
		m_pVs = nullptr;
		delete m_pPs;
		m_pPs = nullptr;
		delete m_pRs;
		m_pRs = nullptr;
		delete m_pPipelineState;
		m_pPipelineState = nullptr;
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

	bool MaterialAsset::UpdateRenderingObjects()
	{
		bool validVertexShader = false;
		if (m_vsBlob.GetSize() != 0)
		{
			delete m_pVs;
			m_pVs = new Rendering::Shader();
			bool res = m_pVs->LoadFromMemory(m_vsBlob);
			if (!res)
				return false;

			validVertexShader = true;
		}

		bool validPixelShader = false;
		if (m_psBlob.GetSize() != 0)
		{
			delete m_pPs;
			m_pPs = new Rendering::Shader();
			bool res = m_pPs->LoadFromMemory(m_psBlob);
			if (!res)
				return false;

			validPixelShader = true;
		}

		bool validRootSignature = false;
		if (m_rsBlob.GetSize() != 0)
		{
			delete m_pRs;
			m_pRs = new Rendering::RootSignature();
			bool res = m_pRs->LoadFromMemory(m_rsBlob);
			if (!res)
				return false;

			validRootSignature = true;
		}

		bool createPipelineState = validPixelShader && validVertexShader && validRootSignature;
		if (!createPipelineState)
			return false;
	
		if (m_pPipelineState)
			delete m_pPipelineState;

		Rendering::PipelineStateDesc desc;
		desc.m_pRs = m_pRs;
		desc.m_pVs = m_pVs;
		desc.m_pPs = m_pPs;
		desc.m_cullMode = m_cullMode;
		desc.m_depthStencilDesc.m_depthFunction = m_depthFunction;

		if (m_blendMode == BlendMode::BM_TRANSLUCENT)
		{
			desc.m_blendDesc.m_blendEnabled = true;
			desc.m_blendDesc.m_blendOperation = Rendering::BlendOperation::ADD;
			desc.m_blendDesc.m_srcBlend = Rendering::BlendFactor::SRC_ALPHA;
			desc.m_blendDesc.m_dstBlend = Rendering::BlendFactor::INV_SRC_ALPHA;
			desc.m_blendDesc.m_blendOperationAlpha = Rendering::BlendOperation::ADD;
			desc.m_blendDesc.m_srcBlendAlpha = Rendering::BlendFactor::ONE;
			desc.m_blendDesc.m_dstBlendAlpha = Rendering::BlendFactor::ZERO;
		}

		m_pPipelineState = new Rendering::PipelineState();
		bool created = m_pPipelineState->Init_Generic(desc);
		if (!created)
		{
			delete m_pPipelineState;
			m_pPipelineState = nullptr;

			return false;
		}
		
		return true;
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

	BlendMode MaterialAsset::GetBlendMode() const
	{
		return m_blendMode;
	}

	Core::Sid MaterialAsset::GetParametersSchemaHash() const
	{
		return m_parametersSchemaHash;
	}

	void MaterialAsset::CalculateParametersSchemaHash()
	{
		m_parametersSchemaHash = CalculateParametersSchemaHash(m_perMaterialParameters, m_texturesBindingInfo);
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

	Core::Sid MaterialAsset::CalculateParametersSchemaHash(const Core::Array<MaterialParameterDescription>& param, const Core::Array<TextureBindingInfo>& textures)
	{
		std::string schema;
		for (const MaterialParameterDescription& desc : param)
		{
			schema += desc.m_name + desc.m_strType;

			size_t offset = schema.size();
			const int DATA_SIZE = sizeof(desc.m_offset);
			schema.resize(schema.size() + DATA_SIZE);
			memcpy(schema.data() + offset, &desc.m_offset, DATA_SIZE);
		}

		for (const TextureBindingInfo& texture : textures)
		{
			schema += texture.m_name;

			{
				size_t offset = schema.size();
				const int DATA_SIZE = sizeof(texture.m_sigRootIndex);
				schema.resize(schema.size() + DATA_SIZE);
				memcpy(schema.data() + offset, &texture.m_sigRootIndex, DATA_SIZE);
			}

			{
				size_t offset = schema.size();
				const int DATA_SIZE = sizeof(texture.m_type);
				schema.resize(schema.size() + DATA_SIZE);
				memcpy(schema.data() + offset, &texture.m_type, DATA_SIZE);
			}
		}

		return Core::MakeSid(schema);
	}
}
