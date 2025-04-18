/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <vector>

#include "Systems/Container/ContainerId.h"

namespace Systems
{
	class AssetObject;

	// A container is a collection of AssetObject. It can be loaded and serialized on disk.
	class Container
	{
	public:
		Container();
		explicit Container(ContainerId cid);
		~Container();

		void SetId(ContainerId cid);
		void AddAsset(AssetObject* pAsset);

		ContainerId GetId() const;
		const std::vector<AssetObject*>& GetAssets() const;
		
		const AssetObject* GetConstAsset(uint64_t objectId) const;
		AssetObject* GetAsset(uint64_t objectId);

	private:
		ContainerId m_id;
		std::vector<AssetObject*> m_assets;
	};
}
