/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	AssetObject::AssetObject(AssetId id)
		: Object()
		, m_id(id)
	{ }

	void AssetObject::SetAssetId(AssetId id)
	{
		m_id = id;
	}
}
