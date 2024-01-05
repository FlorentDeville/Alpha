/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetType.h"

#include "Widgets/Layout.h"

namespace Systems
{
	class AssetId;
}

namespace Widgets
{
	class Button;
	class Container;
	class Label;
	class TableView;
}

namespace Editors
{
	class AssetListModel;

	class AssetIdWidget : public Widgets::Layout
	{
	public:
		AssetIdWidget(Systems::AssetType type);
		~AssetIdWidget();

		void SetValue(const std::string& value);

		EVENT_DECL(AssetSelected, void(Systems::AssetId selectedId))
	private:
		
		Widgets::Container* m_pLabelContainer;
		Widgets::Label* m_pLabel;
		Widgets::Button* m_pButton;

		Widgets::TableView* m_pAssetList;
		AssetListModel* m_pAssetListModel;

		Systems::AssetType m_type;

		void OnOk_AssetList();

		void OnClick_OpenAssetList();
	};
}
