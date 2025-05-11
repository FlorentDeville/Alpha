/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/ModalWindow.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Events/KeyboardEvent.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Message.h"
#include "Widgets/MessageId.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	ModalWindow::ModalWindow(const std::string& title)
		: Container()
	{
		Widgets::ContainerStyle& style = GetDefaultStyle();
		style.SetBorderColor(Color(0.44f, 0.37f, 0.91f, 1.f));
		style.ShowBorder(true);

		m_pInnerLayout = new Widgets::Layout();
		m_pInnerLayout->SetDirection(Widgets::Layout::Vertical);
		m_pInnerLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		
		Container::AddWidget(m_pInnerLayout);

		Container* pLabelContainer = new Container(0, TITLE_HEIGHT);
		pLabelContainer->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pLabelContainer->GetDefaultStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 1.f));
		m_pInnerLayout->AddWidget(pLabelContainer);

		m_pTitleLabel = new Widgets::Label(0, 0, 1, title);
		m_pTitleLabel->SetSize(DirectX::XMUINT2(0, TITLE_HEIGHT));
		

		pLabelContainer->AddWidget(m_pTitleLabel);

		m_pContent = new Widgets::Container();
		m_pContent->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pInnerLayout->AddWidget(m_pContent);
	}

	ModalWindow::~ModalWindow()
	{}

	void ModalWindow::ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		Widgets::Container::ReComputePosition(parentAbsPos, parentSize);
		m_absPos.z = 10;
	}

	bool ModalWindow::Handle(const BaseEvent& ev)
	{
		if (ev.m_id == EventType::kVKeyDown)
		{
			const KeyboardEvent& keyboardEvent = static_cast<const KeyboardEvent&>(ev);
			if(keyboardEvent.m_virtualKey == 0x1B)//escape	
			{
				Widgets::WidgetMgr::Get().CloseModalWindow();
			}
		}

		return true;
	}

	void ModalWindow::AddWidget(Widget* pWidget)
	{
		m_pContent->AddWidget(pWidget);
	}

	void ModalWindow::DeleteChild(Widget* pWidget)
	{
		m_pContent->DeleteChild(pWidget);
	}

	void ModalWindow::Open()
	{
		Widgets::WidgetMgr::Get().OpenModalWindow(this);
	}

	void ModalWindow::Close()
	{
		Widgets::WidgetMgr::Get().CloseModalWindow();
	}
}