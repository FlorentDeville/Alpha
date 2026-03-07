/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Blob/Blob.h"

#include "Systems/Assets/AssetObjects/Texture/ITextureAsset.h"

namespace Rendering
{
	class Texture;
}

ENABLE_REFLECTION(Systems, Texture2DAsset)

namespace Systems
{
	class Texture2DAsset : public ITextureAsset
	{
	public:
		Texture2DAsset();
		~Texture2DAsset();

		void Init(const std::string& sourceFilename, const uint8_t* pBlob, uint32_t blobSize, uint32_t width, uint32_t height, uint32_t mipCount);

		void PostLoad() override;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

		//ITextureAsset interface
		Rendering::Texture* GetTexture() override;
		const Core::Blob& GetBlob() const override;

		const std::string& GetSourceFilename() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetMipCount() const;

	private:

		std::string m_sourceFilename;
		Core::Blob m_blob;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_mipCount;

		START_REFLECTION(Systems::Texture2DAsset)
			ADD_BASETYPE(Systems::ITextureAsset)
			ADD_FIELD(m_sourceFilename)
			ADD_FIELD_ATTR(m_blob, Core::Hidden)
			ADD_FIELD(m_width)
			ADD_FIELD(m_height)
			ADD_FIELD(m_mipCount)
		END_REFLECTION()

		Rendering::Texture* m_pTexture;
	};
}
