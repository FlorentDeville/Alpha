/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

class Widget;

class WidgetMgr
{
public:
	WidgetMgr();
	~WidgetMgr();

	//void RegisterWidget(Widget* pWidget);

	void SetRoot(Widget* pRoot);

	void Draw();

private:

	//std::vector<Widget*> m_widgets;

	Widget* m_pRoot;
};

extern WidgetMgr* g_pWidgetMgr;
