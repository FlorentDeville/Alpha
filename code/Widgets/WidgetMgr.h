/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <deque>
#include <set>


#include "Rendering/Font/Font.h"
#include "Rendering/Material/MaterialId.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Renderable/RenderableId.h"
#include "Rendering/RenderModule.h"

#include "Core/Singleton.h"

struct Message;
class Widget;

namespace Widgets
{
	class Button;
	class Container;
	class Layout;
	class Split;
	class Viewport;
}

class WidgetMgr : public Core::Singleton<WidgetMgr>
{
	friend class Widgets::Button;
	friend class Widgets::Container;
	friend class Widgets::Split;
	friend class Widgets::Viewport;
	friend class Icon;
	friend class Widget;
	friend class Widgets::Layout;

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
	Rendering::MaterialId m_materialId;
	MeshId m_quadMeshId;

	PipelineStateId m_widgetViewportPsoId;
	RenderableId m_iconRenderableId;
	FontId m_segoeUIFontId;

	void ComputeSortedWidgetQueue();

	const Widget* GetFocusedWidget() const;
};
