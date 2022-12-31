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
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/RenderModule.h"

#include "Core/Singleton.h"

struct Message;
class SysWindow;

namespace Widgets
{
	class Button;
	class Icon;
	class Container;
	class Layout;
	class Split;
	class TextBox;
	class Viewport;
	class Widget;

	class WidgetMgrParameter
	{
	public:
		std::string m_gameShaderPath;
		std::string m_editorFontsPath;
		std::string m_editorIconsPath;
		SysWindow* m_pMainWindow;
	};

	class WidgetMgr : public Core::Singleton<WidgetMgr>
	{
		friend class Widgets::Button;
		friend class Widgets::Container;
		friend class Widgets::Icon;
		friend class Widgets::Layout;
		friend class Widgets::Split;
		friend class Widgets::TextBox;
		friend class Widgets::Viewport;
		friend class Widgets::Widget;

	public:
		WidgetMgr();
		~WidgetMgr();

		void Init(const WidgetMgrParameter& parameter);

		void Release();

		void RegisterWidget(Widget* pWidget);
		
		void UnregisterWidget(Widget* pWidget);

		void SetRoot(Widget* pRoot);

		void Update(uint64_t dt);

		void Draw();

		void Resize();

		void HandleMsg(const Message& msg);

		Rendering::FontId GetUIFontId() const;

		//Return the position of the cursor in the entire screen.
		DirectX::XMINT2 GetCursorPosition() const;

		void SetFocus(Widget* pWidget);
		void CaptureMouse(Widget* pWidget);

		void RequestResize();

		const std::string& GetEditorIconsPath() const;

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
		Rendering::MaterialId m_iconMaterialId;
		Rendering::MeshId m_quadMeshId;

		Rendering::PipelineStateId m_widgetViewportPsoId;	//this could be turned into a material
		Rendering::FontId m_segoeUIFontId;

		bool m_resizeRequest;

		SysWindow* m_pMainSysWindow;

		std::string m_editorIconsPath;

		void ComputeSortedWidgetQueue();

		const Widget* GetFocusedWidget() const;
	};
}
