/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class CubemapAsset_v1;
	class CubemapAsset_v2;

	void UpgradeCubemapAssetV1ToV2(const CubemapAsset_v1* pV1, CubemapAsset_v2* pV2);
}
