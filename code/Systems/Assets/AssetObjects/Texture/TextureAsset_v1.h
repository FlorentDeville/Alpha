/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/AssetObject.h"

#include "Rendering/Texture/TextureFormat.h"

namespace Rendering
{
	class Texture;
}

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, TextureAsset_v1)
	class TextureAsset_v1 : public AssetObject
	{
	public:
		TextureAsset_v1();
		~TextureAsset_v1();

		void Init(const std::string& sourceFilename, const uint8_t* pBlob, uint32_t blobSize, uint32_t width, uint32_t height,
			uint32_t mipCount, Rendering::TextureFormat format);

		void PostLoad() override;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

		Rendering::Texture* GetTexture();

	private:

		std::string m_sourceFilename;
		Core::Array<uint8_t> m_blob;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_mipCount;

		//Rendering::TextureFormat m_format;

		START_REFLECTION(Systems::TextureAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFilename)
			ADD_FIELD_ATTR(m_blob, Hidden)
			ADD_FIELD(m_width)
			ADD_FIELD(m_height)
			ADD_FIELD(m_mipCount)
			//ADD_FIELD(m_format)
		END_REFLECTION()

		Rendering::Texture* m_pTexture;
	};
}
