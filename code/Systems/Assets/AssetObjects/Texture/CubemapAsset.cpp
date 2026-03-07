/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Texture/CubemapAsset.h"

#include "Systems/Assets/AssetObjects/Cubemap/CubemapAsset_v2.h"
#include "Rendering/Texture/Texture.h"

namespace Systems
{
	CubemapAsset::CubemapAsset()
		: ITextureAsset()
		, m_leftSourceFilename()
		, m_rightSourceFilename()
		, m_topSourceFilename()
		, m_bottomSourceFilename()
		, m_frontSourceFilename()
		, m_backSourceFilename()
		, m_pTexture(nullptr)
	{ }

	CubemapAsset::~CubemapAsset()
	{
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	bool CubemapAsset::Init(const std::string sourceFilename[6], const uint8_t* pBlob, uint32_t blobSize)
	{
		m_rightSourceFilename = sourceFilename[0];
		m_leftSourceFilename = sourceFilename[1];
		m_topSourceFilename = sourceFilename[2];
		m_bottomSourceFilename = sourceFilename[3];
		m_frontSourceFilename = sourceFilename[4];
		m_backSourceFilename = sourceFilename[5];

		m_blob.Resize(blobSize);
		memcpy(m_blob.GetData(), pBlob, blobSize);

		PostLoad();

		return true;
	}

	void CubemapAsset::PostLoad()
	{
		if (m_pTexture)
			delete m_pTexture;

		m_pTexture = new Rendering::Texture();
		m_pTexture->InitAsDDS(m_blob.GetData(), m_blob.GetSize());
	}

	const std::string& CubemapAsset::GetAssetTypeName()
	{
		static std::string name = "Cubemap";
		return name;
	}

	Core::Sid CubemapAsset::GetAssetTypeNameSid()
	{
		static const Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	const std::string& CubemapAsset::GetLeftSourceFilename() const
	{
		return m_leftSourceFilename;
	}

	const std::string& CubemapAsset::GetRightSourceFilename() const
	{
		return m_rightSourceFilename;
	}

	const std::string& CubemapAsset::GetTopSourceFilename() const
	{
		return m_topSourceFilename;
	}

	const std::string& CubemapAsset::GetBottomSourceFilename() const
	{
		return m_bottomSourceFilename;
	}

	const std::string& CubemapAsset::GetFrontSourceFilename() const
	{
		return m_frontSourceFilename;
	}

	const std::string& CubemapAsset::GetBackSourceFilename() const
	{
		return m_backSourceFilename;
	}

	Rendering::Texture* CubemapAsset::GetTexture()
	{
		return m_pTexture;
	}

	const Core::Blob& CubemapAsset::GetBlob() const
	{
		return m_blob;
	}

	void CubemapAsset::Upgrade(const CubemapAsset_v2* pV1)
	{
		std::string sourceFilenames[6];
		sourceFilenames[0] = pV1->GetRightSourceFilename();
		sourceFilenames[1] = pV1->GetLeftSourceFilename();
		sourceFilenames[2] = pV1->GetTopSourceFilename();
		sourceFilenames[3] = pV1->GetBottomSourceFilename();
		sourceFilenames[4] = pV1->GetFrontSourceFilename();
		sourceFilenames[5] = pV1->GetBackSourceFilename();

		Init(sourceFilenames, pV1->GetBlob().GetData(), pV1->GetBlob().GetSize());
	}
}
