/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/WidgetMgr.h"

#include "Rendering/Font/FontMgr.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Resources/ResourcesMgr.h"

#include "OsWin/Resource.h"
#include "OsWin/SysWindow.h"
#include "OsWin/UIMessage/UIMessage.h"
#include "OsWin/UIMessage/UIMouseMask.h"
#include "OsWin/VirtualKeyCode.h"

#include "Widgets/Events/FocusEvent.h"
#include "Widgets/Events/MouseEvent.h"
#include "Widgets/Shortcuts/Shortcut.h"
#include "Widgets/Widget.h"

#include <algorithm>

//#pragma optimize("", off)

namespace Widgets
{
	WidgetMgr::EventStorage::EventStorage()
	{}

	WidgetMgr::EventStorage::~EventStorage()
	{}

	void CreateRecursiveWidgetsDeque(Widget* pWidget, std::deque<Widget*>& widgetsDeque)
	{
		widgetsDeque.push_back(pWidget);
		for (Widget* pChild : pWidget->GetChildren())
			CreateRecursiveWidgetsDeque(pChild, widgetsDeque);
	}

	WidgetMgr::WidgetMgr()
		: m_pRoot(nullptr)
		, m_prevMouseX(0)
		, m_prevMouseY(0)
		, m_pFocusedWidget(nullptr)
		, m_pCapturedWidget(nullptr)
		, m_widgetViewportPsoId()
		, m_segoeUIFontId()
		, m_resizeRequest(false)
		, m_pMainSysWindow(nullptr)
		, m_pModalWindow(nullptr)
		, m_shortcutsArray()
		, m_internalEvent()
		, m_iconTextureIdArray()
		, m_baseWidgetPsoId()
		, m_iconWidgetPsoId()
		, m_shadowMapPsoId()
	{}

	WidgetMgr::~WidgetMgr()
	{
		if (m_pRoot)
			delete m_pRoot;
	}

	void WidgetMgr::Init(const WidgetMgrParameter& parameter)
	{
		m_pMainSysWindow = parameter.m_pMainWindow;

		Rendering::PipelineStateMgr& pipelineStateMgr = Rendering::PipelineStateMgr::Get();
		Rendering::RootSignatureMgr& rootSignatureMgr = Rendering::RootSignatureMgr::Get();
		Rendering::ShaderMgr& shaderMgr = Rendering::ShaderMgr::Get();

		//Simple quad used to render widgets
		{
			Rendering::VertexPosUv vertices[4] =
			{
				{ DirectX::XMFLOAT3(-0.5f, 0.5f , 0.f), DirectX::XMFLOAT2(0.f, 0.f) },	// top left
				{ DirectX::XMFLOAT3(0.5f , 0.5f , 0.f),	DirectX::XMFLOAT2(1.f, 0.f) },	// top right
				{ DirectX::XMFLOAT3(0.5f , -0.5f, 0.f),	DirectX::XMFLOAT2(1.f, 1.f) },	// bottom right
				{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.f),	DirectX::XMFLOAT2(0.f, 1.f) }	// bottom left
			};

			uint16_t indices[6]
			{
				0, 1, 2,
				2, 3, 0
			};

			Rendering::Mesh* pSimpleQuad = nullptr;
			Rendering::MeshMgr::Get().CreateMesh(&pSimpleQuad, m_quadMeshId);
			pSimpleQuad->LoadVertexAndIndexBuffer(vertices, _countof(vertices), indices, _countof(indices));
		}

		//Root signature for basic widget
		{
			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\widget.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\widget.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\widget.ps.cso");

			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(m_baseWidgetPsoId);
			pPipelineState->Init_PosUv(rsId, vsId, psId);
		}

		const AppResources::ResourcesMgr& resourceMgr = AppResources::ResourcesMgr::Get();

		//Font for label
		{
			std::string fontName = "segoeUI";
			Rendering::Font* pFont = nullptr;
			Rendering::FontMgr::Get().CreateFont(&pFont, m_segoeUIFontId);
			
			//load the font from the application resource
			int16_t fontDefResourceId = resourceMgr.GetSystemResourceId(AppResources::kFontDefSegoeUI);
			const char* fontDefType = resourceMgr.GetSystemResourceType(AppResources::kFontDefSegoeUI);

			char* pFontDefData = nullptr;
			uint32_t fontDefDataSize = 0;
			Os::Resource::GetResource(fontDefResourceId, fontDefType, &pFontDefData, fontDefDataSize);

			int16_t fontTexResourceId = resourceMgr.GetSystemResourceId(AppResources::kFontTexSegoeUI);
			const char* fontTexType = resourceMgr.GetSystemResourceType(AppResources::kFontTexSegoeUI);

			char* pFontTexData = nullptr;
			uint32_t fontTexDataSize = 0;
			Os::Resource::GetResource(fontTexResourceId, fontTexType, &pFontTexData, fontTexDataSize);

			pFont->Init(pFontDefData, fontDefDataSize, pFontTexData, fontTexDataSize);

			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\text.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\text.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\text.ps.cso");

			Rendering::PipelineStateId text_pipelineStateId;
			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(text_pipelineStateId);
			pPipelineState->Init_Text(rsId, vsId, psId);

			Rendering::RenderModule::Get().InitialiseFont(m_segoeUIFontId, text_pipelineStateId, 65535);
		}

		//material for icon
		{
			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\texture.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\texture.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\texture.ps.cso");

			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(m_iconWidgetPsoId);
			pPipelineState->Init_Icon(rsId, vsId, psId);
		}

		//viewport pso
		{
			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\widget_viewport.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\widget_viewport.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\widget_viewport.ps.cso");

			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(m_widgetViewportPsoId);
			pPipelineState->Init_PosUv(rsId, vsId, psId);
		}

		//object id material
		{
			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\object_ids.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\object_ids.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\object_ids.ps.cso");

			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(m_objectIdsPsoId);
			pPipelineState->Init_Generic(rsId, vsId, psId, DXGI_FORMAT_R8G8B8A8_UINT);
		}

		//shadow map material
		{
			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\shadowmap.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\shadowmap.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\shadowmap.ps.cso");

			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(m_shadowMapPsoId);
			pPipelineState->Init_Generic_ShadowMap_SpotLight(rsId, vsId, psId);
		}

		//dir light shadow map material
		{
			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\shadowmap_dirlight.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\shadowmap_dirlight.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\shadowmap_dirlight.ps.cso");

			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(m_shadowMapDirLightPsoId);
			pPipelineState->Init_Generic_ShadowMap_DirLight(rsId, vsId, psId);
		}

		Rendering::TextureMgr& textureMgr = Rendering::TextureMgr::Get();

		//Load expanded icon from resources
		m_iconTextureIdArray[static_cast<int>(IconId::kIconExpanded)] = LoadApplicationResourceImage(AppResources::kUiIconExpanded);
		m_iconTextureIdArray[static_cast<int>(IconId::kIconCollapsed)] = LoadApplicationResourceImage(AppResources::kUiIconCollapsed);
		m_iconTextureIdArray[static_cast<int>(IconId::kIconClose)] = LoadApplicationResourceImage(AppResources::kUiIconClose);
		m_iconTextureIdArray[static_cast<int>(IconId::kIconCloseHover)] = LoadApplicationResourceImage(AppResources::kUiIconCloseHover);
		m_iconTextureIdArray[static_cast<int>(IconId::kIconCursor)] = LoadApplicationResourceImage(AppResources::kUiIconCursor);
		m_iconTextureIdArray[static_cast<int>(IconId::kIconCheckmark)] = LoadApplicationResourceImage(AppResources::kUiIconCheckmark);
	}

	void WidgetMgr::Release()
	{
		Rendering::PipelineStateMgr::Get().DeletePipelineState(m_widgetViewportPsoId);

		Rendering::TextureMgr& textureMgr = Rendering::TextureMgr::Get();
		for (int ii = 0; ii < static_cast<int>(IconId::kCount); ++ii)
		{
			textureMgr.DeleteTexture(m_iconTextureIdArray[ii]);
		}
	}

	void WidgetMgr::RegisterWidget(Widget* pWidget)
	{
		m_widgets.insert(pWidget);
	}

	void WidgetMgr::UnregisterWidget(Widget* pWidget)
	{
		m_widgets.erase(pWidget);

		std::deque<Widget*>::iterator it = std::find(m_sortedWidgets.begin(), m_sortedWidgets.end(), pWidget);
		if(it != m_sortedWidgets.end())
			m_sortedWidgets.erase(it);

		if (m_pFocusedWidget == pWidget)
			SetFocus(nullptr);
	}

	void WidgetMgr::RegisterShortcut(Shortcut* pShortcut)
	{
		m_shortcutsArray.push_back(pShortcut);
	}

	void WidgetMgr::UnregisterShortcut(Shortcut* pShortcut)
	{
		std::vector<Shortcut*>::iterator it = std::find(m_shortcutsArray.begin(), m_shortcutsArray.end(), pShortcut);
		if (it == m_shortcutsArray.end())
			return;

		m_shortcutsArray.erase(it);
	}

	void WidgetMgr::SetRoot(Widget* pRoot)
	{
		m_pRoot = pRoot;
		RegisterWidget(pRoot);
	}

	void WidgetMgr::Update(uint64_t dt)
	{
		for (Widget* pWidget : m_deleteRequestArray)
		{
			//first make sure the widget still exists. This is in the case a child widget is deleted before its parent.
			std::set<Widget*>::const_iterator it = std::find(m_widgets.cbegin(), m_widgets.cend(), pWidget);
			if (it == m_widgets.cend())
				continue;

			if (Widget* pParent = pWidget->GetParent())
				pParent->DeleteChild(pWidget);
		}
		
		m_deleteRequestArray.clear();

		if (m_resizeRequest)
		{
			Resize();
			m_resizeRequest = false;
		}

		for (Widget* pWidget : m_widgets)
			pWidget->Update(dt);
	}

	void WidgetMgr::Render()
	{
		for (Widget* pWidget : m_widgets)
		{
			if(pWidget->IsEnabled())
				pWidget->Internal_Render();
		}
	}

	void WidgetMgr::Draw()
	{
		if (m_pRoot && m_pRoot->IsEnabled())
		{
			Core::Float2 windowSize;
			windowSize.x = static_cast<float>(m_pRoot->GetSize().x);
			windowSize.y = static_cast<float>(m_pRoot->GetSize().y);

			D3D12_RECT scissor;
			scissor.left = 0;
			scissor.right = static_cast<LONG>(windowSize.x);
			scissor.top = 0;
			scissor.bottom = static_cast<LONG>(windowSize.y);

			m_pRoot->Draw(windowSize, scissor);
		}
	}

	void WidgetMgr::Resize()
	{
		if (!m_pRoot)
			return;

		Core::Int3 absPos(0, 0, 100);
		Core::UInt2 size(m_pMainSysWindow->GetWidth(), m_pMainSysWindow->GetHeight());
		m_pRoot->Resize(absPos, size);

		ComputeSortedWidgetQueue();
	}

	void WidgetMgr::HandleMsg(const OsWin::UIMessage& msg)
	{
		std::deque<Widget*>* widgetsSortedQueue = &m_sortedWidgets;

		std::deque<Widget*> capturedQueue;
		if (m_pCapturedWidget) //Send first to the control who requested to capture events.
		{
			capturedQueue.push_back(m_pCapturedWidget);
			widgetsSortedQueue = &capturedQueue;
		}
		else if (m_pModalWindow)
		{
			widgetsSortedQueue = &m_sortedWidgetsModal;
		}
		switch (msg.m_id)
		{
		case OsWin::MouseMove:
		{
			bool hasExited = false;
			bool hasMoved = false; //you can only move or enter in a single frame. So I use the same flag here.

			for (std::deque<Widget*>::reverse_iterator it = widgetsSortedQueue->rbegin(); it != widgetsSortedQueue->rend(); ++it)
			{
				Widget* pWidget = *it;
				if (!pWidget->IsEnabled())
					continue;

				const BaseEvent& ev = ConvertMessageToEvent(pWidget, msg);

				switch (ev.m_id)
				{
				case EventType::kMouseEnter:
				case EventType::kMouseMove:
				{
					if (hasMoved)
					{
						continue;
					}
					else
					{
						hasMoved = pWidget->Handle(ev);
					}
				}
				break;

				case EventType::kMouseExit:
				{
					if (hasExited)
					{
						continue;
					}
					else
					{
						hasExited = pWidget->Handle(ev);
					}
				}
				break;
				
				case EventType::kUnknown:
					break;

				default:
					assert(false);
					break;
				}
				
				if (hasExited && hasMoved)
					break;
			}
			m_prevMouseX = msg.m_low.m_uint32[0];
			m_prevMouseY = msg.m_low.m_uint32[1];
		}
		break;

		case OsWin::MouseLDown:
		case OsWin::MouseLUp:
		case OsWin::MouseMDown:
		case OsWin::MouseMUp:
		case OsWin::MouseRDown:
		case OsWin::MouseRUp:
		case OsWin::MouseLDoubleClick:
		{
			bool setFocus = false;
			for (std::deque<Widget*>::reverse_iterator it = widgetsSortedQueue->rbegin(); it != widgetsSortedQueue->rend(); ++it)
			{
				Widget* pWidget = *it;
				if (!pWidget->IsEnabled())
					continue;

				const BaseEvent& ev = ConvertMessageToEvent(pWidget, msg);

				if (ev.m_id != EventType::kUnknown)
				{
					if (!setFocus)
					{
						SetFocus(pWidget);
						setFocus = true;
					}

					bool handled = pWidget->Handle(ev);
					if (handled)
						break;
				}
			}
		}
		break;

		case OsWin::VirtualKeyDown:
		{
			//First try to handle the inputs, then check if it's a shortcut.
			//Shortcuts need to be checked after regular inputs. For example a TextBox wants to handle
			//keyboard events first and not trigger a shortcut
			bool handled = false;

			if (m_pFocusedWidget)
			{
				const BaseEvent& ev = ConvertMessageToEvent(m_pFocusedWidget, msg);
				if (ev.m_id != EventType::kUnknown)
				{
					Widget* pWidget = m_pFocusedWidget;
					
					while (pWidget && !handled)
					{
						handled = pWidget->Handle(ev);
						pWidget = pWidget->GetParent();
					}
				}
			}

			if (!handled)
			{
				for (Shortcut* pShortcut : m_shortcutsArray)
				{
					pShortcut->Evaluate();
				}
			}
		}
		break;

		case OsWin::VirtualKeyUp:
		case OsWin::CharKeyDown:
		{
			//The first 32 characters of the ascii table are unprintable characters, so ignore them.
			const int UNPRINTABLE_ASCII_CHARACTER = 32;
			if (msg.m_high < UNPRINTABLE_ASCII_CHARACTER)
				break;

			if (m_pFocusedWidget)
			{
				const BaseEvent& ev = ConvertMessageToEvent(m_pFocusedWidget, msg);
				if (ev.m_id != EventType::kUnknown)
					m_pFocusedWidget->Handle(ev);
			}
		}
			break;

		default:
			assert(false);
			break;
		}
	}

	Rendering::FontId WidgetMgr::GetUIFontId() const
	{
		return m_segoeUIFontId;
	}

	DirectX::XMINT2 WidgetMgr::GetCursorPosition() const
	{
		POINT cursorPosition;
		GetCursorPos(&cursorPosition);

		return DirectX::XMINT2(cursorPosition.x, cursorPosition.y);
	}

	void WidgetMgr::SetFocus(Widget* pWidget)
	{
		if (pWidget)
		{
			while (pWidget->GetFocusPolicy() == Widget::FOCUS_POLICY::NO_FOCUS)
				pWidget = pWidget->GetParent();
		}

		if (pWidget == m_pFocusedWidget)
			return;

		FocusEvent ev(m_pFocusedWidget, pWidget);

		if (m_pFocusedWidget && m_pFocusedWidget->m_onFocusLost)
			m_pFocusedWidget->m_onFocusLost(ev);

		if (pWidget && pWidget->m_onFocusGained)
			pWidget->m_onFocusGained(ev);

		m_pFocusedWidget = pWidget;
	}

	void WidgetMgr::CaptureMouse(Widget* pWidget)
	{
		if (pWidget)
		{
			//You can't have 2 widgets requesting to capture events
			assert(!m_pCapturedWidget);
		}

		m_pCapturedWidget = pWidget;
	}

	void WidgetMgr::RequestResize()
	{
		m_resizeRequest = true;
	}

	void WidgetMgr::RequestWidgetDeletion(Widget* pWidget)
	{
		m_deleteRequestArray.push_back(pWidget);
	}

	void WidgetMgr::OpenModalWindow(Widget* pModalWindow)
	{
		m_pModalWindow = pModalWindow;

		m_pRoot->AddWidget(m_pModalWindow);
		Widgets::WidgetMgr::Get().RequestResize();
		SetFocus(m_pModalWindow);
	}

	void WidgetMgr::CloseModalWindow()
	{
		SetFocus(m_pRoot);

		//Don't delete the modal window here. Some event might still be running, so delay it for the next iteration
		RequestWidgetDeletion(m_pModalWindow);
		m_pModalWindow = nullptr;

		RequestResize();
	}

	Rendering::TextureId WidgetMgr::GetIconTextureId(IconId iconId) const
	{
		return m_iconTextureIdArray[static_cast<int>(iconId)];
	}

	Rendering::PipelineStateId WidgetMgr::GetBaseWidgetPsoId() const
	{
		return m_baseWidgetPsoId;
	}

	Rendering::PipelineStateId WidgetMgr::GetIconWidgetPsoId() const
	{
		return m_iconWidgetPsoId;
	}

	Rendering::MeshId WidgetMgr::GetQuadMeshId() const
	{
		return m_quadMeshId;
	}

	Rendering::PipelineStateId WidgetMgr::GetObjectIdsPsoId() const
	{
		return m_objectIdsPsoId;
	}

	Rendering::PipelineStateId WidgetMgr::GetShadowMapPsoId() const
	{
		return m_shadowMapPsoId;
	}

	Rendering::PipelineStateId WidgetMgr::GetShadowMapDirLightPsoId() const
	{
		return m_shadowMapDirLightPsoId;
	}

	void WidgetMgr::ComputeSortedWidgetQueue()
	{
		m_sortedWidgets.clear();

		for (Widget* pWidget : m_widgets)
			m_sortedWidgets.push_back(pWidget);

		std::sort(m_sortedWidgets.begin(), m_sortedWidgets.end(), [](Widget* pA, Widget* pB) { return pA->m_absPos.z > pB->m_absPos.z; });

		if (m_pModalWindow)
		{
			m_sortedWidgetsModal.clear();
			CreateRecursiveWidgetsDeque(m_pModalWindow, m_sortedWidgetsModal);
			
			std::sort(m_sortedWidgetsModal.begin(), m_sortedWidgetsModal.end(), [](Widget* pA, Widget* pB) { return pA->m_absPos.z > pB->m_absPos.z; });
		}
	}

	const Widget* WidgetMgr::GetFocusedWidget() const
	{
		return m_pFocusedWidget;
	}

	const BaseEvent& WidgetMgr::ConvertMessageToEvent(const Widget* pWidget, const OsWin::UIMessage& msg)
	{
		switch (msg.m_id)
		{
		case OsWin::MouseMove:
		{
				bool isInside = pWidget->IsInside(msg.m_low.m_uint32[0], msg.m_low.m_uint32[1]);
				bool wasInside = pWidget->IsInside(m_prevMouseX, m_prevMouseY);

				MouseButton button = MouseButton::NoButton;
				if (msg.m_high & OsWin::UIMouseMask::LeftButton) button = static_cast<MouseButton>(button | MouseButton::LeftButton);
				if (msg.m_high & OsWin::UIMouseMask::RightButton) button = static_cast<MouseButton>(button | MouseButton::RightButton);
				if (msg.m_high & OsWin::UIMouseMask::MiddleButton) button = static_cast<MouseButton>(button | MouseButton::MiddleButton);
				
				if (!wasInside && isInside)
				{
					m_internalEvent.m_mouseEvent = MouseEvent(EventType::kMouseEnter, msg.m_low.m_uint32[0], msg.m_low.m_uint32[1], button);
					return m_internalEvent.m_mouseEvent;
				}
				else if (wasInside && !isInside)
				{
					m_internalEvent.m_mouseEvent = MouseEvent(EventType::kMouseExit, msg.m_low.m_uint32[0], msg.m_low.m_uint32[1], button);
					return m_internalEvent.m_mouseEvent;
				}
				else if (isInside)
				{
					m_internalEvent.m_mouseEvent = MouseEvent(EventType::kMouseMove, msg.m_low.m_uint32[0], msg.m_low.m_uint32[1], button);
					return m_internalEvent.m_mouseEvent;
				}
				else if (pWidget == m_pCapturedWidget) //the mouse is outside, only make an event if we captured the mouse
				{
					m_internalEvent.m_mouseEvent = MouseEvent(EventType::kMouseMove, msg.m_low.m_uint32[0], msg.m_low.m_uint32[1], button);
					return m_internalEvent.m_mouseEvent;
				}
				else //this message doesn't concern this widget
				{
					m_internalEvent.m_baseEvent.m_id = EventType::kUnknown;
					return m_internalEvent.m_baseEvent;
				}
		}
		break;

		case OsWin::MouseLDown:
		case OsWin::MouseLUp:
		case OsWin::MouseMDown:
		case OsWin::MouseMUp:
		case OsWin::MouseRDown:
		case OsWin::MouseRUp:
		case OsWin::MouseLDoubleClick:
		{
			bool isInside = pWidget->IsInside(msg.m_low.m_uint32[0], msg.m_low.m_uint32[1]);
			bool isCaptured = m_pCapturedWidget == pWidget;

			if (isInside || isCaptured)
			{
				MouseButton currentButton = MouseButton::NoButton;
				EventType currentId = EventType::kUnknown;
				switch (msg.m_id)
				{
				case OsWin::MouseLDown:
					currentId = EventType::kMouseDown;
					currentButton = MouseButton::LeftButton;
					break;

				case OsWin::MouseLUp:
					currentId = EventType::kMouseUp;
					currentButton = MouseButton::LeftButton;
					break;

				case OsWin::MouseMDown:
					currentId = EventType::kMouseDown;
					currentButton = MouseButton::MiddleButton;
					break;

				case OsWin::MouseMUp:
					currentId = EventType::kMouseUp;
					currentButton = MouseButton::MiddleButton;
					break;

				case OsWin::MouseRDown:
					currentId = EventType::kMouseDown;
					currentButton = MouseButton::RightButton;
					break;

				case OsWin::MouseRUp:
					currentId = EventType::kMouseUp;
					currentButton = MouseButton::RightButton;
					break;

				case OsWin::MouseLDoubleClick:
					currentId = EventType::kMouseDoubleClick;
					currentButton = MouseButton::LeftButton;
					break;
				}	
				
				m_internalEvent.m_mouseEvent = MouseEvent(currentId, msg.m_low.m_uint32[0], msg.m_low.m_uint32[1], currentButton);
				return m_internalEvent.m_mouseEvent;
			}
			else
			{
				m_internalEvent.m_baseEvent.m_id = EventType::kUnknown;
				return m_internalEvent.m_baseEvent;
			}
		}
		break;

		case OsWin::VirtualKeyDown:
		case OsWin::VirtualKeyUp:
		case OsWin::CharKeyDown:
		{
			if (m_pFocusedWidget == pWidget)
			{
				m_internalEvent.m_keyboardEvent.m_virtualKey = static_cast<char>(msg.m_high);

				switch (msg.m_id)
				{
				case OsWin::VirtualKeyDown:
					m_internalEvent.m_keyboardEvent.m_id = EventType::kVKeyDown;
					break;

				case OsWin::VirtualKeyUp:
					m_internalEvent.m_keyboardEvent.m_id = EventType::kVKeyUp;
					break;

				case OsWin::CharKeyDown:
					m_internalEvent.m_keyboardEvent.m_id = EventType::kCharKeyDown;
					break;
				}
				return m_internalEvent.m_keyboardEvent;
			}
			else
			{
				m_internalEvent.m_baseEvent.m_id = EventType::kUnknown;
				return m_internalEvent.m_baseEvent;
			}
		}
		break;

		default:
			assert(false);
		break;
		}

		m_internalEvent.m_baseEvent.m_id = EventType::kUnknown;
		return m_internalEvent.m_baseEvent;
	}

	Rendering::TextureId WidgetMgr::LoadApplicationResourceImage(AppResources::AppResourceId id) const
	{
		Rendering::TextureMgr& textureMgr = Rendering::TextureMgr::Get();
		const AppResources::ResourcesMgr& resourceMgr = AppResources::ResourcesMgr::Get();

		Rendering::TextureId textureId;
		Rendering::Texture* pTexture = nullptr;
		textureMgr.CreateTexture(&pTexture, textureId);

		char* pData = nullptr;
		uint32_t dataSize = 0;
		int16_t sysId = resourceMgr.GetSystemResourceId(id);
		const char* type = resourceMgr.GetSystemResourceType(id);
		Os::Resource::GetResource(sysId, type, &pData, dataSize);
		pTexture->Init(pData, dataSize);

		return textureId;
	}
}
