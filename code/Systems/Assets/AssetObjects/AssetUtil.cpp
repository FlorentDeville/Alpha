/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/AssetUtil.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Container/ContainerMgr.h"

namespace Systems
{
	bool AssetUtil::DeleteAsset(NewAssetId id)
	{
		//delete the container
		bool res = ContainerMgr::Get().DeleteContainer(id.GetContainerId());
		if (!res)
			return false;

		//delete the metadata
		AssetMgr& assetMgr = AssetMgr::Get();
		res = assetMgr.DeleteMetadata(id);
		if (!res)
			return false;

		res = assetMgr.SaveMetadataTable();
		return res;
	}
}
