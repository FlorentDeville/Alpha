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
	{ }

	CubemapAsset_v1::~CubemapAsset_v1()
	{ }
}
