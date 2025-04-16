/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Container/Container.h"

#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	Container::Container()
		: m_id()
		, m_assets()
	{ }

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

	void Container::SetId(ContainerId cid)
	{
		m_id = cid;
	}

	void Container::AddAsset(AssetObject* pAsset)
	{
		m_assets.push_back(pAsset);
	}

	ContainerId Container::GetId() const
	{
		return m_id;
	}

	const std::vector<AssetObject*>& Container::GetAssets() const
	{
		return m_assets;
	}
}
