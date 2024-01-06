/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetType.h"
#include "Widgets/Models/AbstractViewModel.h"

#include <vector>

namespace Editors
{
	class AssetListModel : public Widgets::AbstractViewModel
	{
	public:
		AssetListModel(Systems::AssetType type);
		~AssetListModel();

		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& child) const override;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override;
		int GetRowCount(const Widgets::ModelIndex& parent) override;
		int GetColumnCount(const Widgets::ModelIndex& parent) const override;

		std::string GetData(const Widgets::ModelIndex& idx) override;

		void InsertRows(int start, int count);

	private:
		Systems::AssetType m_assetType;

		std::vector<std::string> m_cachedAssetId;

		static std::string s_invalidValue;
	};
}
