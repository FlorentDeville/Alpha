/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/ModalWindow.h"

#include "Widgets/Message.h"
#include "Widgets/MessageId.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	ModalWindow::ModalWindow()
		: Container()
	{
		Widgets::ContainerStyle& style = GetDefaultStyle();
		style.SetBorderColor(DirectX::XMVectorSet(0.44f, 0.37f, 0.91f, 1.f));
		style.ShowBorder(true);
	}

	ModalWindow::~ModalWindow()
	{}

	void ModalWindow::ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		Widgets::Container::ReComputePosition(parentAbsPos, parentSize);
		m_absPos.z = 10;
	}

	bool ModalWindow::Handle(const Message& msg)
	{
		if (msg.m_id == MessageId::M_VirtualKeyDown)
		{
			if(msg.m_high == 0x1B)//escape	
			{
				Widgets::WidgetMgr::Get().CloseModalWindow();
			}
		}

		return true;
	}

}