/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"
#include "Systems/Objects/AssetObject.h"

namespace Rendering
{
	class PipelineState;
	class RootSignature;
	class Shader;
}

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, MaterialAsset_v1)
	class MaterialAsset_v1 : public AssetObject
	{
	public:
		MaterialAsset_v1();
		~MaterialAsset_v1();

		const std::string& GetSourceFilePs() const;
		const std::string& GetSourceFileVs() const;

		const Rendering::RootSignature* GetRootSignature() const;
		const Rendering::PipelineState* GetPipelineState() const;

		Core::Array<char>& GetPsBlob();
		Core::Array<char>& GetVsBlob();
		Core::Array<char>& GetRsBlob();

		Core::Array<MaterialParameterDescription>& GetMaterialParameterDescription();
		const Core::Array<MaterialParameterDescription>& GetMaterialParameterDescription() const;

		void PostLoad() override;

		void UpdateRenderingObjects();

		bool IsValidForRendering() const;

		void SetHasPerObjectParameters(bool enable);
		void SetHasPerFrameParameters(bool enable);
		void SetHasLightsParameters(bool enable);

		bool HasPerObjectParameters() const;
		bool HasPerFrameParameters() const;
		bool HasLightsParameters() const;

		void SetPerFrameRootSignatureParameterIndex(uint32_t index);
		void SetPerObjectRootSignatureParameterIndex(uint32_t index);
		void SetPerMaterialRootSignatureParameterIndex(uint32_t index);
		void SetLightsRootSignatureParameterIndex(uint32_t index);

		uint32_t GetPerFrameRootSignatureParameterIndex() const;
		uint32_t GetPerObjectRootSignatureParameterIndex() const;
		uint32_t GetPerMaterialRootSignatureParameterIndex() const;
		uint32_t GetLightsRootSignatureParameterIndex() const;

	private:
		std::string m_sourceFilePS;
		std::string m_sourceFileVS;

		//Not editable variables
		Core::Array<char> m_psBlob;
		Core::Array<char> m_vsBlob;
		Core::Array<char> m_rsBlob;

		bool m_hasPerObjectParameters;
		uint32_t m_perObjectCBufferRootSignatureIndex;

		bool m_hasPerFrameParameters;
		uint32_t m_perFrameCBufferRootSignatureIndex;

		bool m_hasLightsParameters;
		int32_t m_lightsCBufferRootSignatureIndex;

		Core::Array<MaterialParameterDescription> m_perMaterialParameters;
		uint32_t m_perMaterialCBufferRootSignatureIndex;

		//No reflected variables
		Rendering::Shader* m_pVs;
		Rendering::Shader* m_pPs;
		Rendering::RootSignature* m_pRs;
		Rendering::PipelineState* m_pPipelineState;

		START_REFLECTION(Systems::MaterialAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFilePS)
			ADD_FIELD(m_sourceFileVS)
			ADD_FIELD_ATTR(m_psBlob, DoNotEdit)
			ADD_FIELD_ATTR(m_vsBlob, DoNotEdit)
			ADD_FIELD_ATTR(m_rsBlob, DoNotEdit)
			ADD_FIELD_ATTR(m_hasPerObjectParameters, DoNotEdit)
			ADD_FIELD_ATTR(m_perObjectCBufferRootSignatureIndex, DoNotEdit)
			ADD_FIELD_ATTR(m_hasPerFrameParameters, DoNotEdit)
			ADD_FIELD_ATTR(m_perFrameCBufferRootSignatureIndex, DoNotEdit)
			ADD_FIELD_ATTR(m_hasLightsParameters, DoNotEdit)
			ADD_FIELD_ATTR(m_lightsCBufferRootSignatureIndex, DoNotEdit)
			ADD_FIELD(m_perMaterialParameters)
			ADD_FIELD_ATTR(m_perMaterialCBufferRootSignatureIndex, DoNotEdit)

		END_REFLECTION()
	};

	using MaterialAsset = MaterialAsset_v1;
}
