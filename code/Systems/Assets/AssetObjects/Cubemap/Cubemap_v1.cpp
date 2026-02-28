/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Cubemap/Cubemap_v1.h"

#include "Rendering/Texture/Texture.h"

namespace Systems
{
	CubemapAsset_v1::CubemapAsset_v1()
		: AssetObject()
		, m_leftSourceFilename()
		, m_rightSourceFilename()
		, m_topSourceFilename()
		, m_bottomSourceFilename()
		, m_frontSourceFilename()
		, m_backSourceFilename()
		, m_pTexture(nullptr)
	{ }

	CubemapAsset_v1::~CubemapAsset_v1()
	{ }

	bool CubemapAsset_v1::Init(const std::string sourceFilename[6], const uint8_t* pBlob, uint32_t blobSize)
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

	void CubemapAsset_v1::PostLoad()
	{
		if (m_pTexture)
			delete m_pTexture;

		m_pTexture = new Rendering::Texture();
		m_pTexture->InitAsDDS(m_blob.GetData(), m_blob.GetSize());
	}

	const std::string& CubemapAsset_v1::GetAssetTypeName()
	{
		static std::string name = "Cubemap";
		return name;
	}

	Core::Sid CubemapAsset_v1::GetAssetTypeNameSid()
	{
		static const Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

	const std::string& CubemapAsset_v1::GetLeftSourceFilename() const
	{
		return m_leftSourceFilename;
	}

	const std::string& CubemapAsset_v1::GetRightSourceFilename() const
	{
		return m_rightSourceFilename;
	}

	const std::string& CubemapAsset_v1::GetTopSourceFilename() const
	{
		return m_topSourceFilename;
	}

	const std::string& CubemapAsset_v1::GetBottomSourceFilename() const
	{
		return m_bottomSourceFilename;
	}

	const std::string& CubemapAsset_v1::GetFrontSourceFilename() const
	{
		return m_frontSourceFilename;
	}

	const std::string& CubemapAsset_v1::GetBackSourceFilename() const
	{
		return m_backSourceFilename;
	}

	Rendering::Texture* CubemapAsset_v1::GetTexture()
	{
		return m_pTexture;
	}
}
