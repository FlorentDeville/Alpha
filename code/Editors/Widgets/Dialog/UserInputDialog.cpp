/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Dialog/UserInputDialog.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	UserInputDialog::UserInputDialog(const std::string& title)
		: Widgets::ModalWindow(title)
	{
		const int WINDOW_WIDTH = 500;
		const int WINDOW_HEIGHT = 70;
		const int OK_CANCEL_HEIGHT = 50;

		SetSize(DirectX::XMUINT2(WINDOW_WIDTH, WINDOW_HEIGHT));
		SetSizeStyle(Widgets::Widget::DEFAULT);
		SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical);
		pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		AddWidget(pVLayout);

		//input
		m_pTextBox = new Widgets::TextBox();
		m_pTextBox->SetSize(DirectX::XMUINT2(0, 20));
		m_pTextBox->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		m_pTextBox->OnValidate([this](const std::string& text) { m_onInputValidated(text); });
		m_pTextBox->OnEscape([this]() { Close(); });
		pVLayout->AddWidget(m_pTextBox);

		//ok cancel button
		Widgets::Layout* pHLayout = new Widgets::Layout();
		pHLayout->SetDirection(Widgets::Layout::Horizontal);
		pHLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pVLayout->AddWidget(pHLayout);

		Widgets::Button* pOkButton = new Widgets::Button(250, OK_CANCEL_HEIGHT, 0, 0);
		Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, "Ok");
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

	UserInputDialog::~UserInputDialog()
	{ }

	void UserInputDialog::Open()
	{
		Widgets::WidgetMgr::Get().OpenModalWindow(this);
		m_pTextBox->SetFocus();
	}

	void UserInputDialog::Close() const
	{
		Widgets::WidgetMgr::Get().CloseModalWindow();
	}

	void UserInputDialog::OnOk()
	{
		m_onInputValidated(m_pTextBox->GetText());

		Close();
	}

	void UserInputDialog::OnCancel()
	{
		Close();
	}
}
