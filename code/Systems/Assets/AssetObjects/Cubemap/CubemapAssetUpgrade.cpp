/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Cubemap/CubemapAssetUpgrade.h"

#include "Systems/Assets/AssetObjects/Cubemap/CubemapAsset_v2.h"

namespace Systems
{
	void UpgradeCubemapAssetV1ToV2(const CubemapAsset_v1* pV1, CubemapAsset_v2* pV2)
	{
		pV2->Upgrade(pV1);
	}
}
