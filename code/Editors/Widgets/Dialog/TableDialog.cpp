/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/Dialog/TableDialog.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/TableView.h"

namespace Editors
{
	TableDialog::TableDialog(const std::string& title, Widgets::AbstractViewModel* pModel)
		: Widgets::ModalWindow(title)
	{
		const int WINDOW_WIDTH = 500;
		const int WINDOW_HEIGHT = 500;
		const int NEW_ASSET_HEIGHT = 20;
		const int OK_CANCEL_HEIGHT = 30;

		int TABLE_HEIGHT = WINDOW_HEIGHT - OK_CANCEL_HEIGHT;

		SetSize(Core::UInt2(WINDOW_WIDTH, WINDOW_HEIGHT));
		SetSizeStyle(Widgets::DEFAULT);
		SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical_Reverse);
		pVLayout->SetSizeStyle(Widgets::STRETCH);
		AddWidget(pVLayout);

		{
			//button ok escape
			Widgets::Layout* pHLayout = new Widgets::Layout();
			pHLayout->SetDirection(Widgets::Layout::Horizontal);
			pHLayout->SetSizeStyle(Widgets::HSTRETCH_VFIT);
			pVLayout->AddWidget(pHLayout);

			std::string okLabel = "Select";
			Widgets::Button* pOkButton = new Widgets::Button(250, OK_CANCEL_HEIGHT, 0, 0);
			Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, okLabel);
			pOkLabel->SetSizeStyle(Widgets::FIT);
			pOkLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
			pOkButton->AddWidget(pOkLabel);
			pOkButton->SetSizeStyle(Widgets::DEFAULT);
			pOkButton->OnClick([this]() { OnOk(); });

			pHLayout->AddWidget(pOkButton);

			Widgets::Button* pCancelButton = new Widgets::Button(250, OK_CANCEL_HEIGHT, 0, 0);
			Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "Cancel");
			pCancelLabel->SetSizeStyle(Widgets::FIT);
			pCancelLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
			pCancelButton->AddWidget(pCancelLabel);
			pCancelButton->SetSizeStyle(Widgets::DEFAULT);
			pCancelButton->OnClick([this]() { OnCancel(); });
			pHLayout->AddWidget(pCancelButton);
		}

		//list
		m_pTable = new Widgets::TableView();
		m_pTable->SetSizeStyle(Widgets::STRETCH);
		m_pTable->OnItemDoubleClick([this](const Widgets::ModelIndex& index) { OnOk(); });
		m_pTable->SetModel(pModel);

		pVLayout->AddWidget(m_pTable);
	}

	TableDialog::~TableDialog()
	{
	}

	void TableDialog::Open()
	{
		Widgets::WidgetMgr::Get().OpenModalWindow(this);
	}

	void TableDialog::Close() const
	{
		Widgets::WidgetMgr::Get().CloseModalWindow();
	}

	void TableDialog::OnOk()
	{
		const Widgets::AbstractViewModel* pModel = m_pTable->GetModel();
		if (!pModel)
			return;

		const std::list<Widgets::SelectionRow>& selection = pModel->GetSelectionModel()->GetSelectedRows();
		if (selection.empty())
			return;

		Widgets::ModelIndex index = selection.front().GetStartIndex();
		m_onOk(index);

		Close();
	}

	void TableDialog::OnCancel()
	{
		Close();
	}
}
