/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/AssetUtil.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Container/Container.h"

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

	AssetObject* AssetUtil::GetAsset(NewAssetId id)
	{
		Container* pContainer = ContainerMgr::Get().GetContainer(id.GetContainerId());
		if (!pContainer)
			return nullptr;

		AssetObject* pAsset = pContainer->GetAsset(id.GetObjectId());
		return pAsset;
	}

	AssetObject* AssetUtil::LoadAsset(NewAssetId id)
	{
		ContainerMgr& containerMgr = ContainerMgr::Get();
		Container* pContainer = containerMgr.GetContainer(id.GetContainerId());
		if (!pContainer)
		{
			pContainer = containerMgr.LoadContainer(id.GetContainerId());
			if (!pContainer)
				return nullptr;
		}
			
		AssetObject* pAsset = pContainer->GetAsset(id.GetObjectId());
		return pAsset;
	}
}
