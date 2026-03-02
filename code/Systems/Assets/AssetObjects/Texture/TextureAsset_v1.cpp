/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Texture/TextureAsset_v1.h"

#include "Rendering/Texture/Texture.h"

namespace Systems
{
	TextureAsset_v1::TextureAsset_v1()
		: AssetObject()
		, m_sourceFilename()
		, m_blob()
		, m_width(0)
		, m_height(0)
		, m_mipCount(0)
		//, m_format(Rendering::TextureFormat::Unknown)
		, m_pTexture(nullptr)
	{ }

	TextureAsset_v1::~TextureAsset_v1()
	{
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	void TextureAsset_v1::Init(const std::string& sourceFilename, const uint8_t* pBlob, uint32_t blobSize, uint32_t width, uint32_t height,
		uint32_t mipCount, Rendering::TextureFormat format)
	{
		m_sourceFilename = sourceFilename;
		m_blob.Resize(blobSize);
		memcpy(m_blob.GetData(), pBlob, blobSize);

		m_width = width;
		m_height = height;
		m_mipCount = mipCount;

		//m_format = format;

		PostLoad();
	}

	void TextureAsset_v1::PostLoad()
	{
		if (m_pTexture)
			delete m_pTexture;

		m_pTexture = new Rendering::Texture();
		m_pTexture->InitAsDDS(m_blob.GetData(), m_blob.GetSize());
	}

	const std::string& TextureAsset_v1::GetAssetTypeName()
	{
		static std::string name = "Texture";
		return name;
	}

	Core::Sid TextureAsset_v1::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	Rendering::Texture* TextureAsset_v1::GetTexture()
	{
		return m_pTexture;
	}

	const std::string& TextureAsset_v1::GetSourceFilename() const
	{
		return m_sourceFilename;
	}

	const Core::Array<uint8_t>& TextureAsset_v1::GetBlob() const
	{
		return m_blob;
	}
}
