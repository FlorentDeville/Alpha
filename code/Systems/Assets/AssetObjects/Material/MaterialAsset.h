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

		void PostLoad() override;

		void UpdateRenderingObjects();

		bool IsValidForRendering() const;

		void SetHasPerObjectParameters(bool enable);
		void SetHasPerFrameParameters(bool enable);

	private:
		std::string m_sourceFilePS;
		std::string m_sourceFileVS;

		//Not editable variables
		Core::Array<char> m_psBlob;
		Core::Array<char> m_vsBlob;
		Core::Array<char> m_rsBlob;

		bool m_hasPerObjectParameters;
		bool m_hasPerFrameParameters;
		Core::Array<MaterialParameterDescription> m_perMaterialParameters;

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
			ADD_FIELD_ATTR(m_hasPerFrameParameters, DoNotEdit)
			ADD_FIELD(m_perMaterialParameters)
		END_REFLECTION()
	};

	using MaterialAsset = MaterialAsset_v1;
}
