/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <deque>
#include <set>
#include <vector>


#include "Rendering/Font/Font.h"
#include "Rendering/Material/MaterialId.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/RenderModule.h"

#include "Resources/AppResourceId.h"

#include "Core/Singleton.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Events/KeyboardEvent.h"
#include "Widgets/Events/MouseEvent.h"

struct Message;
class SysWindow;

namespace Widgets
{
	class Button;
	class Icon;
	class Container;
	class Layout;
	class Shortcut;
	class Split;
	class TextBox;
	class Viewport;
	class Viewport_v2;
	class Widget;

	class WidgetMgrParameter
	{
	public:
		std::string m_gameShaderPath;
		std::string m_editorFontsPath;
		std::string m_editorIconsPath;
		SysWindow* m_pMainWindow;
	};

	//Manager for all the widgets.
	//The order of execution in each frame is HandleMsg, Update and finally Render.
	//HandleMsg converts inputs to widgets callbacks (onclick, onmove, etc).
	class WidgetMgr : public Core::Singleton<WidgetMgr>
	{
		friend class Widgets::Button;
		friend class Widgets::Container;
		friend class Widgets::Icon;
		friend class Widgets::Layout;
		friend class Widgets::Split;
		friend class Widgets::TextBox;
		friend class Widgets::Viewport;
		friend class Widgets::Viewport_v2;
		friend class Widgets::Widget;

	public:
		WidgetMgr();
		~WidgetMgr();

		void Init(const WidgetMgrParameter& parameter);

		void Release();

		void RegisterWidget(Widget* pWidget);	
		void UnregisterWidget(Widget* pWidget);

		void RegisterShortcut(Shortcut* pShortcut);
		void UnregisterShortcut(Shortcut* pShortcut);

		void SetRoot(Widget* pRoot);

		void Update(uint64_t dt);

		void Render();

		void Draw();

		void Resize();

		//Handle messages coming from windows
		void HandleMsg(const Message& msg);

		Rendering::FontId GetUIFontId() const;

		//Return the position of the cursor in the entire screen.
		DirectX::XMINT2 GetCursorPosition() const;

		void SetFocus(Widget* pWidget);
		void CaptureMouse(Widget* pWidget);

		void RequestResize();
		void RequestWidgetDeletion(Widget* pWidget);

		const std::string& GetEditorIconsPath() const;

		void OpenModalWindow(Widget* pModalWindow);
		void CloseModalWindow();

		Rendering::TextureId GetExpandedIcon() const;
		Rendering::TextureId GetCollapsedIcon() const;
		Rendering::TextureId GetCloseIcon() const;

	private:

		std::set<Widget*> m_widgets;
		std::deque<Widget*> m_sortedWidgets; //sorted from the deepest to the highest.
		std::deque<Widget*> m_sortedWidgetsModal; //sorted from the deepest to the highest.

		Widget* m_pFocusedWidget;	//Widget currently getting the focus. It will receive the keyboard event
		Widget* m_pCapturedWidget;	//Widget current requesting to capture the mouse event.

		Widget* m_pRoot;
		Widget* m_pModalWindow;

		int32_t m_prevMouseX;
		int32_t m_prevMouseY;

		//Ids of resources used to render widgets
		Rendering::MaterialId m_materialId;
		Rendering::MaterialId m_iconMaterialId;
		Rendering::MeshId m_quadMeshId;

		Rendering::PipelineStateId m_widgetViewportPsoId;	//this could be turned into a material
		Rendering::FontId m_segoeUIFontId;

		Rendering::TextureId m_expandedIconTextureId;
		Rendering::TextureId m_collapsedIconTextureId;
		Rendering::TextureId m_closeIconTextureId;

		bool m_resizeRequest;
		std::vector<Widget*> m_deleteRequestArray;

		SysWindow* m_pMainSysWindow;

		std::string m_editorIconsPath;

		//internal event storage
		union EventStorage
		{
			EventStorage();
			~EventStorage();

			BaseEvent m_baseEvent;
			KeyboardEvent m_keyboardEvent;
			MouseEvent m_mouseEvent;
		};
		
		EventStorage m_internalEvent;

		std::vector<Shortcut*> m_shortcutsArray;


		void ComputeSortedWidgetQueue();

		const Widget* GetFocusedWidget() const;

		const BaseEvent& ConvertMessageToEvent(const Widget* pWidget, const Message& msg);

		Rendering::TextureId LoadApplicationResourceImage(AppResources::AppResourceId id) const;
	};
}
