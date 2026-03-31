/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

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

		static AssetObject* GetAsset(NewAssetId id, LoadingDomain domain);

		static AssetObject* LoadAsset(NewAssetId id, LoadingDomain domain);

		static void UnloadAsset(NewAssetId id, LoadingDomain domain);

		template<class T> static T* GetAsset(NewAssetId id, LoadingDomain domain);

		template<class T> static T* LoadAsset(NewAssetId id, LoadingDomain domain);

		template<class T> static T* CreateAsset(const std::string& virtualName);

		template<class T> static bool IsA(NewAssetId id);
	};
}

#include "Systems/Assets/AssetObjects/AssetUtil.inl"