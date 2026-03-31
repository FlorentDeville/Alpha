/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

namespace Systems
{
	template<class T> T* AssetUtil::GetAsset(NewAssetId id, LoadingDomain domain)
	{
		return static_cast<T*>(GetAsset(id, domain));
	}

	template<class T> T* AssetUtil::LoadAsset(NewAssetId id, LoadingDomain domain)
	{
		return static_cast<T*>(LoadAsset(id, domain));
	}

	template<class T> T* AssetUtil::CreateAsset(const std::string& virtualName)
	{
		T* pAsset = Systems::CreateNewAsset<T>();
		if (!pAsset)
			return nullptr;

		AssetMgr& assetMgr = AssetMgr::Get();
		ContainerId cid = assetMgr.GenerateNewContainerId();

		ContainerMgr& containerMgr = ContainerMgr::Get();
		Container* pContainer = containerMgr.CreateContainer(cid);
		pContainer->AddAsset(pAsset);
		bool res = containerMgr.SaveContainer(pContainer->GetId());
		if (!res)
			return nullptr;

		AssetMetadata materialMetadata(pAsset->GetId(), virtualName, T::GetAssetTypeNameSid());

		res = assetMgr.RegisterAssetMetadata(materialMetadata);
		if (!res)
			return nullptr;

		res = assetMgr.SaveMetadataTable();
		if (!res)
			return nullptr;

		return pAsset;
	}

	template<class T> bool AssetUtil::IsA(NewAssetId id)
	{
		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
			return false;

		if (pMetadata->GetAssetType() == T::GetAssetTypeNameSid())
			return true;

		return false;
	}
}
