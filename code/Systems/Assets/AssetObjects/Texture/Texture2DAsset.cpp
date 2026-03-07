/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Texture/Texture2DAsset.h"

#include "Systems/Assets/AssetObjects/Texture/TextureAsset_v2.h"

#include "Rendering/Texture/Texture.h"

namespace Systems
{
	Texture2DAsset::Texture2DAsset()
		: ITextureAsset()
		, m_sourceFilename()
		, m_blob()
		, m_width(0)
		, m_height(0)
		, m_mipCount(0)
		, m_pTexture(nullptr)
	{ }

	Texture2DAsset::~Texture2DAsset()
	{
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	void Texture2DAsset::Init(const std::string& sourceFilename, const uint8_t* pBlob, uint32_t blobSize, uint32_t width, uint32_t height,
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

	void Texture2DAsset::PostLoad()
	{
		if (m_pTexture)
			delete m_pTexture;

		m_pTexture = new Rendering::Texture();
		m_pTexture->InitAsDDS(m_blob.GetData(), m_blob.GetSize());
	}

	const std::string& Texture2DAsset::GetAssetTypeName()
	{
		static std::string name = "Texture";
		return name;
	}

	Core::Sid Texture2DAsset::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	Rendering::Texture* Texture2DAsset::GetTexture()
	{
		return m_pTexture;
	}

	const std::string& Texture2DAsset::GetSourceFilename() const
	{
		return m_sourceFilename;
	}

	const Core::Blob& Texture2DAsset::GetBlob() const
	{
		return m_blob;
	}

	uint32_t Texture2DAsset::GetWidth() const
	{
		return m_width;
	}

	uint32_t Texture2DAsset::GetHeight() const
	{
		return m_height;
	}

	uint32_t Texture2DAsset::GetMipCount() const
	{
		return m_mipCount;
	}

	void Texture2DAsset::Upgrade(const TextureAsset_v2* pV1)
	{
		Init(pV1->GetSourceFilename(), pV1->GetBlob().GetData(), pV1->GetBlob().GetSize(),
			pV1->GetWidth(), pV1->GetHeight(), pV1->GetMipCount());
	}
}
