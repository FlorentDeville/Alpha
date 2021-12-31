/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

#include "Rendering/Font/Font.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Renderable/RenderableId.h"

struct Message;
class Widget;

class WidgetMgr
{
	friend class Button;
	friend class Label;
	friend class Widget;
	friend class HLayout;
public:
	WidgetMgr();
	~WidgetMgr();

	void Init();

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

	//Ids of resources used to render widgets
	MeshId m_quadMeshId;
	PipelineStateId m_widgetPsoId;
	RenderableId m_widgetRenderableId;
	FontId m_segoeUIFontId;
};

extern WidgetMgr* g_pWidgetMgr;
