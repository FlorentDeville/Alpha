/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/Material/MaterialAssetUpgrade.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset_v1.h"
#include "Systems/Assets/AssetObjects/Material/MaterialAsset_v2.h"

namespace Systems
{
	void UpgradeMaterialAssetV1ToV2(const MaterialAsset_v1* pV1, MaterialAsset_v2* pV2)
	{
		pV2->Upgrade(pV1);
	}
}
