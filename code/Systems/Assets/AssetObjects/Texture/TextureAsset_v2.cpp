/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Texture/TextureAsset_v2.h"

#include "Systems/Assets/AssetObjects/Texture/TextureAsset_v1.h"

#include "Rendering/Texture/Texture.h"

namespace Systems
{
	TextureAsset_v2::TextureAsset_v2()
		: AssetObject()
		, m_sourceFilename()
		, m_blob()
		, m_width(0)
		, m_height(0)
		, m_mipCount(0)
		, m_pTexture(nullptr)
	{ }

	TextureAsset_v2::~TextureAsset_v2()
	{
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	void TextureAsset_v2::Init(const std::string& sourceFilename, const uint8_t* pBlob, uint32_t blobSize, uint32_t width, uint32_t height,
		uint32_t mipCount)
	{
		m_sourceFilename = sourceFilename;
		m_blob.Resize(blobSize);
		memcpy(m_blob.GetData(), pBlob, blobSize);

		m_width = width;
		m_height = height;
		m_mipCount = mipCount;

		PostLoad();
	}

	void TextureAsset_v2::PostLoad()
	{
		if (m_pTexture)
			delete m_pTexture;

		m_pTexture = new Rendering::Texture();
		m_pTexture->InitAsDDS(m_blob.GetData(), m_blob.GetSize());
	}

	const std::string& TextureAsset_v2::GetAssetTypeName()
	{
		static std::string name = "Texture";
		return name;
	}

	Core::Sid TextureAsset_v2::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	Rendering::Texture* TextureAsset_v2::GetTexture()
	{
		return m_pTexture;
	}

	const std::string& TextureAsset_v2::GetSourceFilename() const
	{
		return m_sourceFilename;
	}

	const Core::Array<uint8_t>& TextureAsset_v2::GetBlob() const
	{
		return m_blob;
	}

	uint32_t TextureAsset_v2::GetWidth() const
	{
		return m_width;
	}

	uint32_t TextureAsset_v2::GetHeight() const
	{
		return m_height;
	}

	uint32_t TextureAsset_v2::GetMipCount() const
	{
		return m_mipCount;
	}

	void TextureAsset_v2::Upgrade(const TextureAsset_v1* pV1)
	{
		Init(pV1->GetSourceFilename(), pV1->GetBlob().GetData(), pV1->GetBlob().GetSize(),
			pV1->GetWidth(), pV1->GetHeight(), pV1->GetMipCount());
	}
}
