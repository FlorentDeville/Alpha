/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Dialog/AssetDialog.h"

#include "Editors/Widgets/List/Models/AssetListModel.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/TextBox.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/TableView.h"

#include <list>

namespace Editors
{
	AssetDialog::AssetDialog(bool isSaveDialog, Systems::AssetType type)
		: Widgets::ModalWindow(isSaveDialog ? "Save Asset" : "Load Asset")
		, m_assetType(type)
	{
		const int WINDOW_WIDTH = 500;
		const int WINDOW_HEIGHT = 500;
		const int NEW_ASSET_HEIGHT = 20;
		const int OK_CANCEL_HEIGHT = 50;

		int TABLE_HEIGHT = 0;
		if (isSaveDialog)
			TABLE_HEIGHT = WINDOW_HEIGHT - NEW_ASSET_HEIGHT - OK_CANCEL_HEIGHT;
		else
			TABLE_HEIGHT = WINDOW_HEIGHT - OK_CANCEL_HEIGHT;

		SetSize(DirectX::XMUINT2(WINDOW_WIDTH, WINDOW_HEIGHT));
		SetSizeStyle(Widgets::Widget::DEFAULT);
		SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical);
		pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		AddWidget(pVLayout);

		//list
		m_pAssetView = new Widgets::TableView();
		m_pAssetView->SetSize(DirectX::XMUINT2(500, TABLE_HEIGHT));
		m_pAssetView->OnItemDoubleClick([this](const Widgets::ModelIndex& index) { OnOk(); });

		m_pAssetViewModel = new Editors::AssetListModel(type);
		m_pAssetView->SetModel(m_pAssetViewModel);
		pVLayout->AddWidget(m_pAssetView);

		if (isSaveDialog)
		{
			int LABEL_WIDTH = 70;
			int BUTTON_WIDTH = 60;
			int TEXTBOX_WIDTH = WINDOW_WIDTH - LABEL_WIDTH - BUTTON_WIDTH;

			Widgets::Layout* pNewAssetLayout = new Widgets::Layout();
			pNewAssetLayout->SetDirection(Widgets::Layout::Horizontal);
			pNewAssetLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			pNewAssetLayout->SetSize(DirectX::XMUINT2(LABEL_WIDTH, NEW_ASSET_HEIGHT));

			pVLayout->AddWidget(pNewAssetLayout);

			Widgets::Label* pNewAssetLabel = new Widgets::Label("New asset : ");
			pNewAssetLabel->SetSize(DirectX::XMUINT2(LABEL_WIDTH, NEW_ASSET_HEIGHT));
			pNewAssetLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
			
			Widgets::TextBox* pNewAssetTextBox = new Widgets::TextBox();
			pNewAssetTextBox->SetSize(DirectX::XMUINT2(TEXTBOX_WIDTH, NEW_ASSET_HEIGHT));
			pNewAssetTextBox->SetSizeStyle(Widgets::Widget::DEFAULT);
			pNewAssetTextBox->OnValidate([this](const std::string& value) { OnCreateAsset(value); });

			Widgets::Button* pNewAssetButton = new Widgets::Button("Create", BUTTON_WIDTH, NEW_ASSET_HEIGHT);
			pNewAssetButton->OnClick([this, pNewAssetTextBox]() { OnCreateAsset(pNewAssetTextBox->GetText()); });

			pNewAssetLayout->AddWidget(pNewAssetLabel);
			pNewAssetLayout->AddWidget(pNewAssetTextBox);
			pNewAssetLayout->AddWidget(pNewAssetButton);
		}

		//button ok escape
		Widgets::Layout* pHLayout = new Widgets::Layout();
		pHLayout->SetDirection(Widgets::Layout::Horizontal);
		pHLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pVLayout->AddWidget(pHLayout);

		std::string okLabel = isSaveDialog ? "Save" : "Load";
		Widgets::Button* pOkButton = new Widgets::Button(250, OK_CANCEL_HEIGHT, 0, 0);
		Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, okLabel);
		pOkLabel->SetSizeStyle(Widgets::Widget::FIT);
		pOkLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
		pOkButton->AddWidget(pOkLabel);
		pOkButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pOkButton->OnClick([this]() { OnOk(); });

		pHLayout->AddWidget(pOkButton);

		Widgets::Button* pCancelButton = new Widgets::Button(250, OK_CANCEL_HEIGHT, 0, 0);
		Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "Cancel");
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

	void AssetDialog::OnCreateAsset(const std::string& assetName)
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const Systems::Asset* pNewAsset = assetMgr.CreateAsset(m_assetType, assetName);
		if (!pNewAsset)
			return;

		assetMgr.SaveTableOfContent();

		int start = static_cast<int>(assetMgr.GetAssets(m_assetType).size());
		m_pAssetViewModel->InsertRows(start - 1, 1);
	}
}
