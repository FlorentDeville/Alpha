/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	class AssetUtil
	{
	public:
		static bool DeleteAsset(NewAssetId id);
	};
}
