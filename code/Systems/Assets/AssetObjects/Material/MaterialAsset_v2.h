/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Blob/Blob.h"
#include "Core/Collections/Array.h"

#include "Rendering/PipelineState/CullMode.h"

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

ENABLE_REFLECTION(Systems, MaterialAsset_v2)

namespace Systems
{
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

		Core::Blob& GetPsBlob();
		Core::Blob& GetVsBlob();
		Core::Blob& GetRsBlob();

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

	private:
		std::string m_sourceFilePS;
		std::string m_sourceFileVS;

		//Not editable variables
		Core::Blob m_psBlob_;
		Core::Blob m_vsBlob_;
		Core::Blob m_rsBlob_;

		Core::Blob m_psBlob;
		Core::Blob m_vsBlob;
		Core::Blob m_rsBlob;

		Core::Array<MaterialCBufferBindingInfo> m_bindingInfoArray;

		Core::Array<MaterialParameterDescription> m_perMaterialParameters;

		Core::Array<TextureBindingInfo> m_texturesBindingInfo;

		Rendering::CullMode m_cullMode;

		START_REFLECTION(Systems::MaterialAsset_v2)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFilePS)
			ADD_FIELD(m_sourceFileVS)
			ADD_FIELD_ATTR(m_psBlob_, Core::Hidden | Core::DoNotSerialize)
			ADD_FIELD_ATTR(m_vsBlob_, Core::Hidden | Core::DoNotSerialize)
			ADD_FIELD_ATTR(m_rsBlob_, Core::Hidden | Core::DoNotSerialize)
			ADD_FIELD_ATTR(m_psBlob, Core::Hidden)
			ADD_FIELD_ATTR(m_vsBlob, Core::Hidden)
			ADD_FIELD_ATTR(m_rsBlob, Core::Hidden)
			ADD_FIELD_ATTR(m_bindingInfoArray, Core::Hidden)
			ADD_FIELD(m_perMaterialParameters)
			ADD_FIELD(m_texturesBindingInfo)
			ADD_FIELD(m_cullMode)
		END_REFLECTION()

		//No reflected variables
		Rendering::Shader* m_pVs;
		Rendering::Shader* m_pPs;
		Rendering::RootSignature* m_pRs;
		Rendering::PipelineState* m_pPipelineState;

		bool m_isDebug; //true if the shader is compiled in debug
	};
}
