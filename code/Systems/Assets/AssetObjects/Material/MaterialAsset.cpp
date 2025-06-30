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
		, m_sourceFileRS()
		, m_sourceFileVS()
		, m_pVs(nullptr)
		, m_pPs(nullptr)
		, m_pRs(nullptr)
		, m_pPipelineState(nullptr)
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
}
