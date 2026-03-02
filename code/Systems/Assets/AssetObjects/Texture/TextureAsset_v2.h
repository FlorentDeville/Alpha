/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Blob/Blob.h"

#include "Systems/Objects/AssetObject.h"

#include "Rendering/Texture/TextureFormat.h"

namespace Rendering
{
	class Texture;
}

namespace Systems
{
	class TextureAsset_v1;

	ENABLE_REFLECTION_WITH_NS(Systems, TextureAsset_v2)
	class TextureAsset_v2 : public AssetObject
	{
	public:
		TextureAsset_v2();
		~TextureAsset_v2();

		void Init(const std::string& sourceFilename, const uint8_t* pBlob, uint32_t blobSize, uint32_t width, uint32_t height, uint32_t mipCount);

		void PostLoad() override;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

		Rendering::Texture* GetTexture();

		const std::string& GetSourceFilename() const;

		const Core::Blob& GetBlob() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetMipCount() const;

		void Upgrade(const TextureAsset_v1* pV1);

	private:

		std::string m_sourceFilename;
		Core::Blob m_blob;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_mipCount;

		START_REFLECTION(Systems::TextureAsset_v2)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFilename)
			ADD_FIELD_ATTR(m_blob, Hidden)
			ADD_FIELD(m_width)
			ADD_FIELD(m_height)
			ADD_FIELD(m_mipCount)
		END_REFLECTION()

		Rendering::Texture* m_pTexture;
	};
}
