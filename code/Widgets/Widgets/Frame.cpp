/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/Frame.h"

#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"

#include "Widgets/Tools/DefaultColors.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Frame::Frame(const std::string& title)
		: Container()
	{
		GetDefaultStyle().ShowBorder(true);
		GetDefaultStyle().SetBorderColor(DefaultColors::FrameBorder);

		m_pInternalLayout = new Layout();
		m_pInternalLayout->SetSizeStyle(Widget::STRETCH);
		m_pInternalLayout->SetDirection(Layout::Vertical);
		Container::AddWidget(m_pInternalLayout);

		m_pTitleLayout = new Layout();
		m_pTitleLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_FIT);
		m_pTitleLayout->SetDirection(Layout::Horizontal);
		m_pInternalLayout->AddWidget(m_pTitleLayout);

		m_pInternalContainer = new Container();
		m_pInternalContainer->SetSizeStyle(Widget::STRETCH);
		m_pInternalLayout->AddWidget(m_pInternalContainer);

		//title bar
		m_pTitleLayout->AddWidget(new Container(5, 0));

		m_pTitleLabel = new Label(title);
		m_pTitleLabel->SetSizeStyle(Widget::FIT);
		m_pTitleLayout->AddWidget(m_pTitleLabel);

		Layout* pIconLayout = new Layout();
		pIconLayout->SetDirection(Layout::Horizontal_Reverse);
		pIconLayout->SetSizeStyle(Widget::STRETCH);
		m_pTitleLayout->AddWidget(pIconLayout);

		std::string closeIconPath = WidgetMgr::Get().GetEditorIconsPath() + "/close.png";
		m_pCloseIcon = new Icon(closeIconPath);
		m_closeTextureId = m_pCloseIcon->GetTextureId();

		std::string closeHoverIconPath = WidgetMgr::Get().GetEditorIconsPath() + "/close_hover.png";
		m_pCloseHoverIcon = new Icon(closeHoverIconPath);
		m_closeHoverTextureId = m_pCloseHoverIcon->GetTextureId();

		m_pCloseButton = new Container(m_pCloseIcon->GetWidth(), m_pCloseIcon->GetHeight());
		m_pCloseButton->AddWidget(m_pCloseIcon);
		m_pCloseButton->SetY(5);

		m_pCloseButton->OnMouseEnter([this](const MouseEvent& ev) { OnMouseEnter_CloseButton(); });
		m_pCloseButton->OnMouseExit([this](const MouseEvent& ev) { OnMouseExit_CloseButton(); });
		m_pCloseButton->OnMouseDown([this](const MouseEvent& ev) { OnMouseDown_CloseButton(ev); });

		pIconLayout->AddWidget(m_pCloseButton);
	}

	Frame::~Frame()
	{
		delete m_pCloseHoverIcon;
	}

	void Frame::AddWidget(Widget* pWidget)
	{
		m_pInternalContainer->AddWidget(pWidget);
	}

	void Frame::Close()
	{
		if (m_onClose)
			m_onClose();
		Widgets::WidgetMgr::Get().RequestWidgetDeletion(this); // the widget will be deleted next frame
	}

	Core::CallbackId Frame::OnClose(const OnCloseEvent::Callback& callback)
	{
		return m_onClose.Connect(callback);
	}

	void Frame::OnMouseDown_CloseButton(const MouseEvent& ev)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		Close();
	}

	bool Frame::OnMouseEnter_CloseButton()
	{
		m_pCloseIcon->SetTextureId(m_closeHoverTextureId);
		return true;
	}

	bool Frame::OnMouseExit_CloseButton()
	{
		m_pCloseIcon->SetTextureId(m_closeTextureId);
		return true;
	}
}
