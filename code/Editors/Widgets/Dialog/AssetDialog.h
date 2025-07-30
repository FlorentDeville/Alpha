/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

#include "Widgets/ModalWindow.h"

#include "Systems/Assets/AssetType.h"

namespace Systems
{
	class NewAssetId;
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
		AssetDialog(Core::Sid assetType);
		~AssetDialog();

		void Open();
		void Close() const;
		
		EVENT_DECL(Ok, void(Systems::NewAssetId))

	private:
		Widgets::TableView* m_pAssetView;
		Editors::AssetListModel* m_pAssetViewModel;

		void OnOk();
		void OnCancel();
	};
}
