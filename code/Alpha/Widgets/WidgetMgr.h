/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <set>

#include "Rendering/Font/Font.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Renderable/RenderableId.h"

struct Message;
class Widget;

namespace Widgets
{
	class Container;
}

class WidgetMgr
{
	friend class Button;
	friend class Widgets::Container;
	friend class Icon;
	friend class Label;
	friend class Widget;
	friend class Layout;
public:
	WidgetMgr();
	~WidgetMgr();

	void Init();

	void RegisterWidget(Widget* pWidget);

	void SetRoot(Widget* pRoot);

	void Draw();

	void Resize();

	void HandleMsg(const Message& msg);

	FontId GetUIFontId() const;

private:

	std::set<Widget*> m_widgets;

	Widget* m_pRoot;
	
	int32_t m_prevMouseX;
	int32_t m_prevMouseY;

	//Ids of resources used to render widgets
	MeshId m_quadMeshId;
	PipelineStateId m_widgetPsoId;
	RenderableId m_widgetRenderableId;
	RenderableId m_iconRenderableId;
	FontId m_segoeUIFontId;
};

extern WidgetMgr* g_pWidgetMgr;
