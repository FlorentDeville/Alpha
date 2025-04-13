/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Container/Container.h"

#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	Container::Container(ContainerId cid)
		: m_id(cid)
		, m_assets()
	{ }

	Container::~Container()
	{
		for (AssetObject* pObject : m_assets)
			delete pObject;

		m_assets.clear();
	}

	void Container::AddAsset(AssetObject* pAsset)
	{
		m_assets.push_back(pAsset);
	}
}
