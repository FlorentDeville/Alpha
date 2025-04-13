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
		explicit Container(ContainerId cid);
		~Container();

		void AddAsset(AssetObject* pAsset);

	private:
		ContainerId m_id;
		std::vector<AssetObject*> m_assets;
	};
}
