/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetObjects/Material/MaterialCBufferBindingInfo.h"
#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"
#include "Systems/Assets/AssetObjects/Material/TextureBindingInfo.h"
#include "Systems/Objects/AssetObject.h"

namespace Rendering
{
	class PipelineState;
	class RootSignature;
	class Shader;
}

namespace Systems
{
	class MaterialAsset_v1;

	ENABLE_REFLECTION_WITH_NS(Systems, MaterialAsset_v2)
	class MaterialAsset_v2 : public AssetObject
	{
	public:
		MaterialAsset_v2();
		~MaterialAsset_v2();

		const std::string& GetSourceFilePs() const;
		const std::string& GetSourceFileVs() const;

		void SetSourceFilePs(const std::string& psFilename);
		void SetSourceFileVs(const std::string& vsFilename);

		const Rendering::RootSignature* GetRootSignature() const;
		const Rendering::PipelineState* GetPipelineState() const;

		Core::Array<char>& GetPsBlob();
		Core::Array<char>& GetVsBlob();
		Core::Array<char>& GetRsBlob();

		Core::Array<MaterialParameterDescription>& GetMaterialParameterDescription();
		const Core::Array<MaterialParameterDescription>& GetMaterialParameterDescription() const;

		Core::Array<MaterialCBufferBindingInfo>& GetBindingInfoArray();
		const Core::Array<MaterialCBufferBindingInfo>& GetBindingInfoArray() const;

		Core::Array<TextureBindingInfo>& GetTexturesBindingInfo();
		const Core::Array<TextureBindingInfo>& GetTexturesBindingInfo() const;

		void PostLoad() override;

		void UpdateRenderingObjects();

		bool IsValidForRendering() const;

		void SetIsDebug(bool debug);
		bool IsDebug() const;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

		void Upgrade(const MaterialAsset_v1* pV1);

	private:
		std::string m_sourceFilePS;
		std::string m_sourceFileVS;

		//Not editable variables
		Core::Array<char> m_psBlob;
		Core::Array<char> m_vsBlob;
		Core::Array<char> m_rsBlob;

		Core::Array<MaterialCBufferBindingInfo> m_bindingInfoArray;

		Core::Array<MaterialParameterDescription> m_perMaterialParameters;

		Core::Array<TextureBindingInfo> m_texturesBindingInfo;

		START_REFLECTION(Systems::MaterialAsset_v2)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFilePS)
			ADD_FIELD(m_sourceFileVS)
			ADD_FIELD_ATTR(m_psBlob, Hidden)
			ADD_FIELD_ATTR(m_vsBlob, Hidden)
			ADD_FIELD_ATTR(m_rsBlob, Hidden)
			ADD_FIELD_ATTR(m_bindingInfoArray, Hidden)
			ADD_FIELD(m_perMaterialParameters)
			ADD_FIELD(m_texturesBindingInfo)
		END_REFLECTION()

		//No reflected variables
		Rendering::Shader* m_pVs;
		Rendering::Shader* m_pPs;
		Rendering::RootSignature* m_pRs;
		Rendering::PipelineState* m_pPipelineState;

		bool m_isDebug; //true if the shader is compiled in debug
	};
}
