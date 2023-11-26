/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/Frame.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"

#include "Widgets/Tools/DefaultColors.h"

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
		m_pTitleLayout->AddWidget(m_pTitleLabel);
	}

	Frame::~Frame()
	{}

	void Frame::AddWidget(Widget* pWidget)
	{
		m_pInternalContainer->AddWidget(pWidget);
	}
}
