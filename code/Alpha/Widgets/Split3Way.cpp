/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Split3Way.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Split3Way::Split3Way()
		: Widget()
		, m_pLeftContainer(nullptr)
		, m_pMiddleContainer(nullptr)
		, m_pRightContainer(nullptr)
		, m_pLeftSplit(nullptr)
		, m_pRightSplit(nullptr)
		, m_pLayout(nullptr)
		, m_leftDragged(false)
		, m_rightDragged(false)
		, m_previousMousePosition(0, 0)
	{
		m_pLayout = new Layout(0, 0, 0, 0);
		m_pLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pLayout->SetDirection(Layout::Direction::Horizontal);
		AddWidget(m_pLayout);

		m_pLeftContainer = new Container(400, 0);
		m_pLeftContainer->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pLeftContainer);

		m_pLeftSplit = new Split();
		m_pLeftSplit->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLeftSplit->OnLeftMouseDown([this](int /*x*/, int /*y*/) -> bool {
			m_leftDragged = true;
			m_previousMousePosition = g_pWidgetMgr->GetCursorPosition();
			return true;
			});

		m_pLeftSplit->OnLeftMouseUp([this](int, int) -> bool {
			m_leftDragged = false;
			return true;
			});

		m_pLayout->AddWidget(m_pLeftSplit);

		m_pMiddleContainer = new Container(300, 0);
		m_pMiddleContainer->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pMiddleContainer);

		m_pRightSplit = new Split();
		m_pRightSplit->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pRightSplit->OnLeftMouseDown([this](int /*x*/, int /*y*/) -> bool {
			m_rightDragged = true;
			m_previousMousePosition = g_pWidgetMgr->GetCursorPosition();
			return true;
			});

		m_pRightSplit->OnLeftMouseUp([this](int, int) -> bool {
			m_rightDragged = false;
			return true;
			});

		m_pLayout->AddWidget(m_pRightSplit);

		m_pRightContainer = new Container(200, 0);
		m_pRightContainer->SetSizeStyle(Widget::VSIZE_STRETCH | Widget::HSIZE_STRETCH);
		m_pLayout->AddWidget(m_pRightContainer);
	}

	Split3Way::~Split3Way()
	{}

	void Split3Way::Update()
	{
		if (m_leftDragged)
		{
			DirectX::XMINT2 currentMousePosition = g_pWidgetMgr->GetCursorPosition();

			DirectX::XMINT2 dt;
			dt.x = currentMousePosition.x - m_previousMousePosition.x;
			dt.y = currentMousePosition.y - m_previousMousePosition.y;

			DirectX::XMUINT2 leftContainerSize = m_pLeftContainer->GetSize();
			leftContainerSize.x += dt.x;
			leftContainerSize.y += dt.y;
			m_pLeftContainer->SetSize(leftContainerSize);

			DirectX::XMUINT2 middleContainerSize = m_pMiddleContainer->GetSize();
			middleContainerSize.x -= dt.x;
			middleContainerSize.y -= dt.y;
			m_pMiddleContainer->SetSize(middleContainerSize);

			g_pWidgetMgr->Resize();

			m_previousMousePosition.x = currentMousePosition.x;
			m_previousMousePosition.y = currentMousePosition.y;
		}

		if (m_rightDragged)
		{
			DirectX::XMINT2 currentMousePosition = g_pWidgetMgr->GetCursorPosition();

			DirectX::XMINT2 dt;
			dt.x = currentMousePosition.x - m_previousMousePosition.x;
			dt.y = currentMousePosition.y - m_previousMousePosition.y;

			DirectX::XMUINT2 middleContainerSize = m_pMiddleContainer->GetSize();
			middleContainerSize.x += dt.x;
			middleContainerSize.y += dt.y;
			m_pMiddleContainer->SetSize(middleContainerSize);

			g_pWidgetMgr->Resize();

			m_previousMousePosition.x = currentMousePosition.x;
			m_previousMousePosition.y = currentMousePosition.y;
		}
	}

	void Split3Way::Draw()
	{
		Widget::Draw();
	}
}
