/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <deque>
#include <set>

#include "Rendering/Font/Font.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Renderable/RenderableId.h"

#include "Core/Singleton.h"

struct Message;
class Widget;

namespace Widgets
{
	class Container;
	class Split;
	class Viewport;
}

class WidgetMgr : public Core::Singleton<WidgetMgr>
{
	friend class Button;
	friend class Widgets::Container;
	friend class Widgets::Split;
	friend class Widgets::Viewport;
	friend class Icon;
	friend class Label;
	friend class Widget;
	friend class Layout;

public:
	WidgetMgr();
	~WidgetMgr();

	void Init();

	void Release();

	void RegisterWidget(Widget* pWidget);

	void SetRoot(Widget* pRoot);

	void Update();

	void Draw();

	void Resize();

	void HandleMsg(const Message& msg);

	FontId GetUIFontId() const;

	//Return the position of the cursor in the entire screen.
	DirectX::XMINT2 GetCursorPosition() const;

	void SetFocus(Widget* pWidget);
	void CaptureMouse(Widget* pWidget);

private:

	std::set<Widget*> m_widgets;
	std::deque<Widget*> m_sortedWidgets; //sorted from the deepest to the highest.

	Widget* m_pFocusedWidget;	//Widget currently getting the focus. It will receive the keyboard event
	Widget* m_pCapturedWidget;	//Widget current requesting to capture the mouse event.

 	Widget* m_pRoot;
	
	int32_t m_prevMouseX;
	int32_t m_prevMouseY;

	//Ids of resources used to render widgets
	MeshId m_quadMeshId;
	PipelineStateId m_widgetPsoId;
	PipelineStateId m_widgetViewportPsoId;
	RenderableId m_widgetRenderableId;
	RenderableId m_iconRenderableId;
	FontId m_segoeUIFontId;

	void ComputeSortedWidgetQueue();

	const Widget* GetFocusedWidget() const;
};
