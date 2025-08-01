/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Models/AbstractViewModel.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	class AssetMetadata;
}

namespace Editors
{
	class MaterialListModel : public Widgets::AbstractViewModel
	{
	public:
		enum Columns
		{
			Id = 0,
			Name,
			Base,
			Modified,

			Count
		};

		MaterialListModel();
		~MaterialListModel();

		// Override base class functions
		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& index) const override;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override;
		int GetRowCount(const Widgets::ModelIndex& parent) const override;
		int GetColumnCount(const Widgets::ModelIndex& parent) const override;
		std::string GetData(const Widgets::ModelIndex& index) override;
		std::string GetHeaderData(int column) override;

		// Specific functions
		void AddRow(const Systems::AssetMetadata* pMetadata);
		void RemoveRow(Systems::NewAssetId id);

		void SetShaderModified(Systems::NewAssetId id);
		void ClearShaderModified(Systems::NewAssetId id);

		void OnMaterialRenamed(const Systems::AssetMetadata& metadata);

		Systems::NewAssetId GetAssetId(const Widgets::ModelIndex& index) const;

		Widgets::ModelIndex GetIndex(Systems::NewAssetId id) const;

	private:
		struct CachedShaderData
		{
			Systems::NewAssetId m_id;
			std::string m_virtualName;
			std::string m_baseMaterial; //only for material instance
			bool m_modified;
		};

		std::vector<CachedShaderData> m_cache;

		//Map between base material and all the instance material using it as a base
		std::map<Systems::NewAssetId, Core::Array<Systems::NewAssetId>> m_baseToInstance;

		void AddToCache(const Systems::AssetMetadata* pMetadata);

		int FindCacheIndex(Systems::NewAssetId id) const;
	};
}
