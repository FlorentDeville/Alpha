/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>
#include <vector>

#include "Systems/Container/ContainerId.h"
#include "Systems/Container/LoadingDomain.h"
#include "Systems/Container/ObjectId.h"

namespace Systems
{
	class AssetObject;

	// A container is a collection of AssetObject. It can be loaded and serialized on disk.
	class Container
	{
	public:
		Container();
		Container(ContainerId cid, LoadingDomain domain);
		~Container();

		void SetId(ContainerId cid);
		void AddAsset(AssetObject* pAsset);

		ContainerId GetId() const;
		const std::vector<AssetObject*>& GetAssets() const;
		
		const AssetObject* GetConstAsset(ObjectId objectId) const;
		AssetObject* GetAsset(ObjectId objectId);

		LoadingDomain GetLoadingDomain() const;

	private:
		ContainerId m_id;
		LoadingDomain m_domain;
		std::vector<AssetObject*> m_assets;
	};
}
