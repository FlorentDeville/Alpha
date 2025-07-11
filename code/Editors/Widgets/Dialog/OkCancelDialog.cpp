/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Dialog/OkCancelDialog.h"

#include "OsWin/VirtualKeyCode.h"

#include "Widgets/Button.h"
#include "Widgets/Container.h"
#include "Widgets/Events/EventType.h"
#include "Widgets/Events/KeyboardEvent.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Editors
{
	OkCancelDialog::OkCancelDialog(const std::string& title, const std::string& text)
		: Widgets::ModalWindow(title)
	{
		const int WINDOW_WIDTH = 500;
		const int WINDOW_HEIGHT = 100;

		const int BUTTON_WIDTH = static_cast<int>(WINDOW_WIDTH * 0.5f);
		const int BUTTON_HEIGHT = 30;
		
		const int BUTTON_PADDING_HEIGHT = 2;
		const int LABEL_HEIGHT = WINDOW_HEIGHT - BUTTON_HEIGHT - TITLE_HEIGHT - BUTTON_PADDING_HEIGHT;
		const int LABEL_WIDTH = WINDOW_WIDTH;

		SetSize(Core::UInt2(WINDOW_WIDTH, WINDOW_HEIGHT));
		SetSizeStyle(Widgets::Widget::DEFAULT);
		SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//create layout
		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Vertical, Widgets::Widget::STRETCH);
		AddWidget(pLayout);

		//create a container for the label
		Widgets::Container* pContainer = new Widgets::Container(LABEL_WIDTH, LABEL_HEIGHT);
		pLayout->AddWidget(pContainer);

		//create label
		Widgets::Label* pLabel = new Widgets::Label(text);
		pLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
		pLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pContainer->AddWidget(pLabel);

		//create layout containing ok and cancel buttons
		Widgets::Layout* pButtonLayout = new Widgets::Layout(Widgets::Layout::Horizontal, Widgets::Widget::HSTRETCH_VFIT);
		pLayout->AddWidget(pButtonLayout);

		//create ok button
		Widgets::Button* pOkButton = new Widgets::Button("Ok", BUTTON_WIDTH, BUTTON_HEIGHT);
		pOkButton->OnClick([this]() { Internal_OnOk(); });
		pButtonLayout->AddWidget(pOkButton);

		//create cancel button
		Widgets::Button* pCancelButton = new Widgets::Button("Cancel", BUTTON_WIDTH, BUTTON_HEIGHT);
		pCancelButton->OnClick([this]() { Internal_OnCancel(); });
		pButtonLayout->AddWidget(pCancelButton);
	}

	OkCancelDialog::~OkCancelDialog()
	{ }

	bool OkCancelDialog::Handle(const Widgets::BaseEvent& ev)
	{
		if (ev.m_id == Widgets::EventType::kVKeyDown)
		{
			const Widgets::KeyboardEvent& keyboardEvent = static_cast<const Widgets::KeyboardEvent&>(ev);
			if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Esc)//escape	
			{
				Internal_OnCancel();
			}
			else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Return)
			{
				Internal_OnOk();
			}
		}

		return true;
	}

	void OkCancelDialog::Internal_OnOk()
	{
		m_onOk();
		Close();
	}

	void OkCancelDialog::Internal_OnCancel()
	{
		m_onCancel();
		Close();
	}
}
