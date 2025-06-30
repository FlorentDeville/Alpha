/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	class AssetObject;

	class AssetUtil
	{
	public:
		static bool DeleteAsset(NewAssetId id);

		static AssetObject* GetAsset(NewAssetId id);

		static AssetObject* LoadAsset(NewAssetId id);

		template<class T> static T* GetAsset(NewAssetId id);
	};

	template<class T> T* AssetUtil::GetAsset(NewAssetId id)
	{
		return static_cast<T*>(GetAsset(id));
	}
}
