/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

struct Message;
class Widget;

class WidgetMgr
{
	friend Widget;

public:
	WidgetMgr();
	~WidgetMgr();

	void RegisterWidget(Widget* pWidget);

	void SetRoot(Widget* pRoot);

	void Draw();

	void Resize();

	void HandleMsg(const Message& msg);

private:

	std::vector<Widget*> m_widgets;

	Widget* m_pRoot;
	
	int32_t m_prevMouseX;
	int32_t m_prevMouseY;
};

extern WidgetMgr* g_pWidgetMgr;
