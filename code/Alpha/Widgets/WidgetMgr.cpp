/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/WidgetMgr.h"

#include "Widgets/Message.h"
#include "Widgets/Widget.h"

#include "Window.h"

extern Window* g_pWindow;

WidgetMgr::WidgetMgr()
	: m_pRoot(nullptr)
	, m_prevMouseX(0)
	, m_prevMouseY(0)
{}

WidgetMgr::~WidgetMgr()
{
	if (m_pRoot)
		delete m_pRoot;
}

void WidgetMgr::RegisterWidget(Widget* pWidget)
{
	m_widgets.push_back(pWidget);
}

void WidgetMgr::SetRoot(Widget* pRoot)
{
	m_pRoot = pRoot;
	RegisterWidget(pRoot);
}

void WidgetMgr::Draw()
{
	float windowWidth = static_cast<float>(g_pWindow->GetWidth());
	float windowHeight = static_cast<float>(g_pWindow->GetHeight());
	float x = -windowWidth * 0.5f;
	float y = windowHeight * 0.5f;

	m_pRoot->Draw(static_cast<int32_t>(x), static_cast<int32_t>(y), 10);
}

void WidgetMgr::HandleMsg(const Message& msg)
{
	switch (msg.m_id)
	{
	case M_MouseMove:
	{
		for (Widget* pWidget : m_widgets)
		{
			bool handled = false;
			if(pWidget->IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]))
			{
				if (!pWidget->IsInside(m_prevMouseX, m_prevMouseY)) //if the previous position was outside, send a Entering message
				{
					Message enteringMsg;
					enteringMsg.m_id = M_MouseEnter;
					enteringMsg.m_low.m_raw = msg.m_low.m_raw;

					handled = pWidget->Handle(enteringMsg);
				}
				else
				{
					handled = pWidget->Handle(msg);
				}
			}
			else if (pWidget->IsInside(m_prevMouseX, m_prevMouseY)) //if the previous pos was inside, send a exit message
			{
				Message exitMsg;
				exitMsg.m_id = M_MouseExit;
				exitMsg.m_low.m_raw = msg.m_low.m_raw;

				handled = pWidget->Handle(exitMsg);
			}

			//I can't break here, if I entered a widget, I exited another so I need to loop through everything
			//Once I have a proper way of checking what widget are where, I won't need to iterate like this.
			/*if (handled)
				break;*/
		}

		m_prevMouseX = msg.m_low.m_pos[0];
		m_prevMouseY = msg.m_low.m_pos[1];
	}
		break;
	}
}

WidgetMgr* g_pWidgetMgr = nullptr;
