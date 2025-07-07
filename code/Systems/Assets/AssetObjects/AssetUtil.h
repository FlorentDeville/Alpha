/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Objects/AssetObject.h"

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

		template<class T> static T* LoadAsset(NewAssetId id);

		template<class T> static T* CreateAsset(const std::string& virtualName);
	};

	template<class T> T* AssetUtil::GetAsset(NewAssetId id)
	{
		return static_cast<T*>(GetAsset(id));
	}

	template<class T> T* AssetUtil::LoadAsset(NewAssetId id)
	{
		return static_cast<T*>(LoadAsset(id));
	}

	template<class T> T* AssetUtil::CreateAsset(const std::string& virtualName)
	{
		T* pAsset = Systems::CreateNewAsset<T>();
		if (!pAsset)
			return nullptr;

		ContainerMgr& containerMgr = ContainerMgr::Get();
		Container* pContainer = containerMgr.CreateContainer(virtualName.c_str());
		pContainer->AddAsset(pAsset);
		bool res = containerMgr.SaveContainer(pContainer->GetId());
		if (!res)
			return nullptr;

		AssetMetadata materialMetadata(pAsset->GetId(), virtualName, T::GetAssetTypeNameSid());
		AssetMgr& assetMgr = AssetMgr::Get();
		res = assetMgr.RegisterAssetMetadata(materialMetadata);
		if (!res)
			return nullptr;

		res = assetMgr.SaveMetadataTable();
		if (!res)
			return nullptr;

		return pAsset;
	}
}
