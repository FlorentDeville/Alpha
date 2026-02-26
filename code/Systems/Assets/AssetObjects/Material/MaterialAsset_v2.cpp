/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Material/MaterialAsset_v2.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset_v1.h"

#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/Shaders/Shader.h"

namespace Systems
{
	MaterialAsset_v2::MaterialAsset_v2()
		: m_sourceFilePS()
		, m_sourceFileVS()
		, m_pVs(nullptr)
		, m_pPs(nullptr)
		, m_pRs(nullptr)
		, m_pPipelineState(nullptr)
		, m_isDebug(false)
	{
	}

	MaterialAsset_v2::~MaterialAsset_v2()
	{
		delete m_pVs;
		delete m_pPs;
		delete m_pRs;
		delete m_pPipelineState;
	}

	const std::string& MaterialAsset_v2::GetSourceFilePs() const
	{
		return m_sourceFilePS;
	}

	const std::string& MaterialAsset_v2::GetSourceFileVs() const
	{
		return m_sourceFileVS;
	}

	void MaterialAsset_v2::SetSourceFilePs(const std::string& psFilename)
	{
		m_sourceFilePS = psFilename;
	}

	void MaterialAsset_v2::SetSourceFileVs(const std::string& vsFilename)
	{
		m_sourceFileVS = vsFilename;
	}

	const Rendering::RootSignature* MaterialAsset_v2::GetRootSignature() const
	{
		return m_pRs;
	}

	const Rendering::PipelineState* MaterialAsset_v2::GetPipelineState() const
	{
		return m_pPipelineState;
	}

	Core::Array<char>& MaterialAsset_v2::GetPsBlob()
	{
		return m_psBlob;
	}

	Core::Array<char>& MaterialAsset_v2::GetVsBlob()
	{
		return m_vsBlob;
	}

	Core::Array<char>& MaterialAsset_v2::GetRsBlob()
	{
		return m_rsBlob;
	}

	Core::Array<MaterialParameterDescription>& MaterialAsset_v2::GetMaterialParameterDescription()
	{
		return m_perMaterialParameters;
	}

	const Core::Array<MaterialParameterDescription>& MaterialAsset_v2::GetMaterialParameterDescription() const
	{
		return m_perMaterialParameters;
	}

	Core::Array<MaterialCBufferBindingInfo>& MaterialAsset_v2::GetBindingInfoArray()
	{
		return m_bindingInfoArray;
	}

	const Core::Array<MaterialCBufferBindingInfo>& MaterialAsset_v2::GetBindingInfoArray() const
	{
		return m_bindingInfoArray;
	}

	Core::Array<TextureBindingInfo>& MaterialAsset_v2::GetTexturesBindingInfo()
	{
		return m_texturesBindingInfo;
	}

	void MaterialAsset_v2::PostLoad()
	{
		UpdateRenderingObjects();
	}

	void MaterialAsset_v2::UpdateRenderingObjects()
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

	bool MaterialAsset_v2::IsValidForRendering() const
	{
		return m_pPipelineState && m_pRs;
	}

	void MaterialAsset_v2::SetIsDebug(bool debug)
	{
		m_isDebug = debug;
	}

	bool MaterialAsset_v2::IsDebug() const
	{
		return m_isDebug;
	}

	const std::string& MaterialAsset_v2::GetAssetTypeName()
	{
		static std::string name = "Material";
		return name;
	}

	Core::Sid MaterialAsset_v2::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	void MaterialAsset_v2::Upgrade(const MaterialAsset_v1* pV1)
	{
		m_sourceFilePS = pV1->GetSourceFilePs();
		m_sourceFileVS = pV1->GetSourceFileVs();

		m_psBlob = pV1->m_psBlob;
		m_vsBlob = pV1->m_vsBlob;
		m_rsBlob = pV1->m_rsBlob;

		m_bindingInfoArray = pV1->m_bindingInfoArray;
		m_perMaterialParameters = pV1->m_perMaterialParameters;

		PostLoad();
	}
}
