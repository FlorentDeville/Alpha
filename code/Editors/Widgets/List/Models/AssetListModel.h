/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/BaseModel.h"

#include "Systems/Assets/AssetType.h"

#include <vector>

namespace Editors
{
	class AssetListModel : public BaseModel
	{
	public:
		AssetListModel(Systems::AssetType type);
		~AssetListModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;
		const std::string& GetData(int rowId = 0, int columnId = 0) const override;

	private:
		Systems::AssetType m_assetType;

		std::vector<std::string> m_cachedAssetId;

		static std::string s_invalidValue;
	};
}
