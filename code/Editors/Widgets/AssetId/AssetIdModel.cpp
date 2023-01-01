/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/AssetId/AssetIdModel.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"

namespace Editors
{
	AssetIdModel::AssetIdModel(Systems::AssetId id, PropertyType type)
		: BaseModel()
		, m_id(id)
		, m_cachedValue()
		, m_type(type)
	{}

	AssetIdModel::~AssetIdModel()
	{}

	int AssetIdModel::GetRowCount() const
	{
		return 1;
	}

	int AssetIdModel::GetColumnCount() const
	{
		return 1;
	}

	const std::string& AssetIdModel::GetData(int rowId, int columnId) const
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

	PropertyType AssetIdModel::GetDataType(int /*rowId*/, int /*columnId*/) const
	{
		return m_type;
	}
}
