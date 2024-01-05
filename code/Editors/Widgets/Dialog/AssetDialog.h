/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/ModalWindow.h"

#include "Systems/Assets/AssetType.h"

namespace Systems
{
	class AssetId;
}

namespace Widgets
{
	class TableView;
}

namespace Editors
{
	class AssetListModel;

	class AssetDialog : public Widgets::ModalWindow
	{
	public:
		AssetDialog(bool isSaveDialog, Systems::AssetType type);
		~AssetDialog();

		void Open();
		void Close() const;
		
		EVENT_DECL(AssetSelected, void(Systems::AssetId))

	private:
		Widgets::TableView* m_pAssetView;
		Editors::AssetListModel* m_pAssetViewModel;

		void OnOk();
		void OnCancel();
	};
}
