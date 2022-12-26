/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/AssetId/AssetIdModel.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"

namespace Editors
{
	AssetIdModel::AssetIdModel(Systems::AssetId id)
		: BaseModel()
		, m_id(id)
		, m_cachedValue()
	{}

	AssetIdModel::~AssetIdModel()
	{}

	const std::string& AssetIdModel::GetData() const
	{
		if (!m_cachedValue.empty())
			return m_cachedValue;

		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const Systems::Asset* pAsset = assetMgr.GetAsset(m_id);
		if (!pAsset)
			m_cachedValue = std::string("Unknown Asset");
		else
			m_cachedValue = pAsset->GetVirtualName() + " (" + m_id.ToString() + ")";

		return m_cachedValue;
	}
}
