/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/ModalWindow.h"

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
		Widgets::Widget::ReComputePosition(parentAbsPos, parentSize);
		m_absPos.z = 10;
	}
}