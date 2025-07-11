/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/AssetId/AssetIdWidget.h"

#include "Editors/Widgets/List/Models/AssetListModel.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetId.h"

#include "Widgets/Button.h"
#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/ModalWindow.h"
#include "Widgets/Widgets/TableView.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	AssetIdWidget::AssetIdWidget(Systems::AssetType type)
		: Layout()
		, m_pAssetList(nullptr)
		, m_pAssetListModel(nullptr)
		, m_type(type)
	{
		SetDirection(Widgets::Layout::Horizontal_Reverse);

		m_pButton = new Widgets::Button(20, 20, 0, 0);
		m_pButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		AddWidget(m_pButton);

		Widgets::Label* pButtonLabel = new Widgets::Label(0, 0, 1, "...");
		pButtonLabel->SetSizeStyle(Widgets::Widget::STRETCH);
		pButtonLabel->SetX(6);
		pButtonLabel->SetY(-3);
		m_pButton->AddWidget(pButtonLabel);
		m_pButton->OnClick([this]() { OnClick_OpenAssetList(); });
		
		m_pLabelContainer = new Widgets::Container();
		m_pLabelContainer->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pLabelContainer->GetDefaultStyle().ShowBorder(true);
		m_pLabelContainer->GetDefaultStyle().SetBorderColor(Widgets::Color(0.26f, 0.26f, 0.26f, 1.f));
		m_pLabelContainer->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0.22f, 0.22f, 0.22f, 1.f));
		AddWidget(m_pLabelContainer);

		m_pLabel = new Widgets::Label();
		m_pLabel->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pLabel->SetX(5);
		m_pLabelContainer->AddWidget(m_pLabel);
	}

	AssetIdWidget::~AssetIdWidget()
	{}

	void AssetIdWidget::SetValue(const std::string& value)
	{
		m_pLabel->SetText(value);
	}

	void AssetIdWidget::OnOk_AssetList()
	{
		const std::list<Widgets::SelectionRow>& selection = m_pAssetListModel->GetSelectionModel()->GetSelectedRows();
		if (selection.empty())
			return;

		const void* pData = selection.back().GetStartIndex().GetConstDataPointer();
		const Systems::Asset* pAsset = static_cast<const Systems::Asset*>(pData);
		m_onAssetSelected(pAsset->GetId());

		Widgets::WidgetMgr::Get().CloseModalWindow();
	}

	void AssetIdWidget::OnClick_OpenAssetList()
	{
		Widgets::ModalWindow* pWindow = new Widgets::ModalWindow("Select Asset");
		pWindow->SetSize(Core::UInt2(500, 500));
		pWindow->SetSizeStyle(Widgets::Widget::DEFAULT);
		pWindow->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical);
		pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pWindow->AddWidget(pVLayout);

		//list
		m_pAssetList = new Widgets::TableView();
		m_pAssetList->SetSize(Core::UInt2(500, 450));
		m_pAssetList->OnItemDoubleClick([this](const Widgets::ModelIndex& index) { OnOk_AssetList(); });

		m_pAssetListModel = new Editors::AssetListModel(m_type);
		m_pAssetList->SetModel(m_pAssetListModel);
		pVLayout->AddWidget(m_pAssetList);

		//button ok escape
		Widgets::Layout* pHLayout = new Widgets::Layout();
		pHLayout->SetDirection(Widgets::Layout::Horizontal);
		pHLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pVLayout->AddWidget(pHLayout);

		Widgets::Button* pOkButton = new Widgets::Button(250, 50, 0, 0);
		Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, "OK");
		pOkLabel->SetSizeStyle(Widgets::Widget::FIT);
		pOkLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
		pOkButton->AddWidget(pOkLabel);
		pOkButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pOkButton->OnClick([this]() { OnOk_AssetList(); });

		pHLayout->AddWidget(pOkButton);

		Widgets::Button* pCancelButton = new Widgets::Button(250, 50, 0, 0);
		Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "CANCEL");
		pCancelLabel->SetSizeStyle(Widgets::Widget::FIT);
		pCancelLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
		pCancelButton->AddWidget(pCancelLabel);
		pCancelButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pCancelButton->OnClick([]() -> bool { Widgets::WidgetMgr::Get().CloseModalWindow(); return true; });
		pHLayout->AddWidget(pCancelButton);

		Widgets::WidgetMgr::Get().OpenModalWindow(pWindow);
	}
}
