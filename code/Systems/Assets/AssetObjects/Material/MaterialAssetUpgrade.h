/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class MaterialAsset_v1;
	class MaterialAsset_v2;

	void UpgradeMaterialAssetV1ToV2(const MaterialAsset_v1* pV1, MaterialAsset_v2* pV2);
}
