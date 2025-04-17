/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	AssetObject::AssetObject(NewAssetId id)
		: Object()
		, m_id(id)
	{ }

	void AssetObject::SetAssetId(NewAssetId id)
	{
		m_id = id;
	}

	NewAssetId AssetObject::GetId() const
	{
		return m_id;
	}
}
