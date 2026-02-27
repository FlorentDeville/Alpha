/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Cubemap/Cubemap_v1.h"

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

	}
}
