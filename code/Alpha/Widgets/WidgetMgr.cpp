/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/WidgetMgr.h"

#include "Widgets/Widget.h"

#include "Window.h"

extern Window* g_pWindow;

WidgetMgr::WidgetMgr()
	: m_pRoot(nullptr)
{}

WidgetMgr::~WidgetMgr()
{
	if (m_pRoot)
		delete m_pRoot;
}


void WidgetMgr::SetRoot(Widget* pRoot)
{
	m_pRoot = pRoot;
}

void WidgetMgr::Draw()
{
	float windowWidth = static_cast<float>(g_pWindow->GetWidth());
	float windowHeight = static_cast<float>(g_pWindow->GetHeight());
	float x = -windowWidth * 0.5f;
	float y = windowHeight * 0.5f;

	m_pRoot->Draw(static_cast<int32_t>(x), static_cast<int32_t>(y), 10);
}

WidgetMgr* g_pWidgetMgr = nullptr;
