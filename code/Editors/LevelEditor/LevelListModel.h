/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Widgets/Models/AbstractViewModel.h"

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	class AssetMetadata;
}

namespace Editors
{
	class LevelListModel : public Widgets::AbstractViewModel
	{
	public:
		enum Columns
		{
			Id = 0,
			Name,
			Modified,

			Count
		};

		LevelListModel();
		~LevelListModel();

		// Override base class functions
		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& index) const override;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override;
		int GetRowCount(const Widgets::ModelIndex& parent) const override;
		int GetColumnCount(const Widgets::ModelIndex& parent) const override;
		std::string GetData(const Widgets::ModelIndex& index) override;
		std::string GetHeaderData(int column) override;

		// Specific functions
		void AddNewLevel(const Systems::AssetMetadata& metadata);

		Systems::NewAssetId GetAssetId(const Widgets::ModelIndex& index) const;

	private:
		struct CachedLevelData
		{
			Systems::NewAssetId m_id;
			std::string m_virtualName;
			bool m_modified;
		};

		Core::Array<CachedLevelData> m_cachedDataArray;

		void AddCachedData(const Systems::AssetMetadata& pMetadata);
	};
}
