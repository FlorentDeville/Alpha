/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Dialog/AssetDialog.h"

#include "Editors/Widgets/List/Models/AssetListModel.h"

#include "Systems/Assets/Asset.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/TableView.h"

#include <list>

namespace Editors
{
	AssetDialog::AssetDialog(bool isSaveDialog, Systems::AssetType type)
		: Widgets::ModalWindow(isSaveDialog ? "Save Asset" : "Load Asset")
	{
		SetSize(DirectX::XMUINT2(500, 500));
		SetSizeStyle(Widgets::Widget::DEFAULT);
		SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical);
		pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		AddWidget(pVLayout);

		//list
		m_pAssetView = new Widgets::TableView();
		m_pAssetView->SetSize(DirectX::XMUINT2(500, 450));
		m_pAssetView->OnItemDoubleClick([this](const Widgets::ModelIndex& index) { OnOk(); });

		m_pAssetViewModel = new Editors::AssetListModel(type);
		m_pAssetView->SetModel(m_pAssetViewModel);
		pVLayout->AddWidget(m_pAssetView);

		//button ok escape
		Widgets::Layout* pHLayout = new Widgets::Layout();
		pHLayout->SetDirection(Widgets::Layout::Horizontal);
		pHLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pVLayout->AddWidget(pHLayout);

		std::string okLabel = isSaveDialog ? "SAVE" : "LOAD";
		Widgets::Button* pOkButton = new Widgets::Button(250, 50, 0, 0);
		Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, okLabel);
		pOkLabel->SetSizeStyle(Widgets::Widget::FIT);
		pOkLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
		pOkButton->AddWidget(pOkLabel);
		pOkButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pOkButton->OnClick([this]() { OnOk(); });

		pHLayout->AddWidget(pOkButton);

		Widgets::Button* pCancelButton = new Widgets::Button(250, 50, 0, 0);
		Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "CANCEL");
		pCancelLabel->SetSizeStyle(Widgets::Widget::FIT);
		pCancelLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
		pCancelButton->AddWidget(pCancelLabel);
		pCancelButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pCancelButton->OnClick([this]() { OnCancel(); });
		pHLayout->AddWidget(pCancelButton);
	}

	AssetDialog::~AssetDialog()
	{}

	void AssetDialog::Open()
	{
		Widgets::WidgetMgr::Get().OpenModalWindow(this);
	}

	void AssetDialog::Close() const
	{
		Widgets::WidgetMgr::Get().CloseModalWindow();
	}

	void AssetDialog::OnOk()
	{
		const std::list<Widgets::SelectionRow>& selection = m_pAssetViewModel->GetSelectionModel()->GetSelectedRows();
		if (selection.empty())
			return;

		const void* pData = selection.back().GetStartIndex().GetConstDataPointer();
		const Systems::Asset* pAsset = static_cast<const Systems::Asset*>(pData);
		m_onAssetSelected(pAsset->GetId());

		Close();
	}

	void AssetDialog::OnCancel()
	{
		Close();
	}
}
