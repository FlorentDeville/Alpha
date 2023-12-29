/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

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
}

namespace Editors
{
	class AssetListModel;
	class BaseModel;
	class ListWidget;

	class AssetIdWidget : public Widgets::Layout
	{
	public:
		AssetIdWidget();
		~AssetIdWidget();

		void SetModel(BaseModel* pModel);

		void Update(uint64_t dt) override;

	private:
		BaseModel* m_pModel;

		Widgets::Container* m_pLabelContainer;
		Widgets::Label* m_pLabel;
		Widgets::Button* m_pButton;

		ListWidget* m_pAssetList;
		AssetListModel* m_pAssetListModel;

		void OnOk_AssetList();
	};
}
