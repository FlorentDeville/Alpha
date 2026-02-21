/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Models/AbstractViewModel.h"

#include "Core/Collections/Array.h"

#include "Systems/Assets/NewAssetId.h"

namespace Systems
{
	class AssetMetadata;
}

namespace Editors
{
	class TextureListModel : public Widgets::AbstractViewModel
	{
	public:
		enum Columns
		{
			Id = 0,
			Name,
			Modified,

			Count
		};

		TextureListModel();
		~TextureListModel();

		// Override base class functions
		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& index) const override;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override;
		int GetRowCount(const Widgets::ModelIndex& parent) const override;
		int GetColumnCount(const Widgets::ModelIndex& parent) const override;
		std::string GetData(const Widgets::ModelIndex& index) override;
		std::string GetHeaderData(int column) override;

		// Specific functions
		void AddRow(const Systems::AssetMetadata& metadata);
		void RemoveRow(Systems::NewAssetId id);

		/*void SetTextureModified(Systems::NewAssetId id);
		void ClearTextureModified(Systems::NewAssetId id);
		*/

		void OnTextureRenamed(const Systems::AssetMetadata& metadata);

		Systems::NewAssetId GetAssetId(const Widgets::ModelIndex& index) const;

	private:
		struct CachedTextureData
		{
			Systems::NewAssetId m_id;
			std::string m_virtualName;
			bool m_modified;
		};

		Core::Array<CachedTextureData> m_cache;

		Widgets::ModelIndex GetIndex(Systems::NewAssetId id) const;
	};
}
