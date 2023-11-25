/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/WidgetMgr.h"

#include "Rendering/Font/FontMgr.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/Material/Material.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"

#include "OsWin/SysWindow.h"

#include "Widgets/Message.h"
#include "Widgets/Widget.h"

#include <algorithm>

//#pragma optimize("", off)

namespace Widgets
{
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
		, m_editorIconsPath()
		, m_pMainSysWindow(nullptr)
		, m_pModalWindow(nullptr)
	{}

	WidgetMgr::~WidgetMgr()
	{
		if (m_pRoot)
			delete m_pRoot;
	}

	void WidgetMgr::Init(const WidgetMgrParameter& parameter)
	{
		m_pMainSysWindow = parameter.m_pMainWindow;
		m_editorIconsPath = parameter.m_editorIconsPath;

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

			Rendering::PipelineStateId psoId;
			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(psoId);
			pPipelineState->Init_PosUv(rsId, vsId, psId);

			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
			Rendering::Material* pWidgetMaterial = nullptr;
			materialMgr.CreateMaterial(&pWidgetMaterial, m_materialId);
			pWidgetMaterial->Init(rsId, psoId);
		}

		//Font for label
		{
			std::string fontName = "segoeUI";
			Rendering::Font* pFont = nullptr;
			Rendering::FontMgr::Get().CreateFont(&pFont, m_segoeUIFontId);
			pFont->Init(parameter.m_editorFontsPath, fontName);

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

			Rendering::PipelineStateId texture_posuv_pipelineStateId;
			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(texture_posuv_pipelineStateId);
			pPipelineState->Init_Icon(rsId, vsId, psId);

			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
			Rendering::Material* pMaterial = nullptr;
			materialMgr.CreateMaterial(&pMaterial, m_iconMaterialId);
			pMaterial->Init(rsId, texture_posuv_pipelineStateId);
		}

		//this should be a material
		{
			Rendering::RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\widget_viewport.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\widget_viewport.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(parameter.m_gameShaderPath + "\\widget_viewport.ps.cso");

			Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(m_widgetViewportPsoId);
			pPipelineState->Init_PosUv(rsId, vsId, psId);
		}
	}

	void WidgetMgr::Release()
	{
		Rendering::PipelineStateMgr::Get().DeletePipelineState(m_widgetViewportPsoId);
	}

	void WidgetMgr::RegisterWidget(Widget* pWidget)
	{
		m_widgets.insert(pWidget);
	}

	void WidgetMgr::UnregisterWidget(Widget* pWidget)
	{
		m_widgets.erase(pWidget);
	}

	void WidgetMgr::SetRoot(Widget* pRoot)
	{
		m_pRoot = pRoot;
		RegisterWidget(pRoot);
	}

	void WidgetMgr::Update(uint64_t dt)
	{
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
			pWidget->Internal_Render();
	}

	void WidgetMgr::Draw()
	{
		if (m_pRoot && m_pRoot->IsEnabled())
		{
			DirectX::XMFLOAT2 windowSize;
			windowSize.x = static_cast<float>(m_pRoot->GetSize().x);
			windowSize.y = static_cast<float>(m_pRoot->GetSize().y);
			m_pRoot->Draw(windowSize);
		}
	}

	void WidgetMgr::Resize()
	{
		if (!m_pRoot)
			return;

		DirectX::XMINT3 absPos(0, 0, 100);
		DirectX::XMUINT2 size(m_pMainSysWindow->GetWidth(), m_pMainSysWindow->GetHeight());
		m_pRoot->Resize(absPos, size);

		ComputeSortedWidgetQueue();
	}

	void WidgetMgr::HandleMsg(const Message& msg)
	{
		//Send first to the control who requested to capture events.
		if (m_pCapturedWidget)
		{
			bool handled = m_pCapturedWidget->Handle(msg);
			if (handled)
				return;
		}

		std::deque<Widget*>* widgetsSortedQueue = &m_sortedWidgets;
		if (m_pModalWindow)
			widgetsSortedQueue = &m_sortedWidgetsModal;

		switch (msg.m_id)
		{
		case M_MouseMove:
		{
			bool hasExited = false;
			bool hasMoved = false; //you can only move or enter in a single frame. So I use the same flag here.

			for (std::deque<Widget*>::reverse_iterator it = widgetsSortedQueue->rbegin(); it != widgetsSortedQueue->rend(); ++it)
			{
				Widget* pWidget = *it;
				if (!pWidget->IsEnabled())
					continue;

				Message ev = ConvertMessageToEvent(pWidget, msg);

				switch (ev.m_id)
				{
				case MessageId::M_MouseEnter:
				case MessageId::M_MouseMove:
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

				case MessageId::M_MouseExit:
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
				
				case MessageId::M_Invalid:
					break;

				default:
					assert(false);
					break;
				}
				
				if (hasExited && hasMoved)
					break;
			}
			m_prevMouseX = msg.m_low.m_pos[0];
			m_prevMouseY = msg.m_low.m_pos[1];
		}
		break;

		case M_MouseLDown:
		case M_MouseLUp:
		case M_MouseMDown:
		case M_MouseMUp:
		case M_MouseRDown:
		case M_MouseRUp:
		{
			bool setFocus = false;
			for (std::deque<Widget*>::reverse_iterator it = widgetsSortedQueue->rbegin(); it != widgetsSortedQueue->rend(); ++it)
			{
				Widget* pWidget = *it;
				if (!pWidget->IsEnabled())
					continue;

				if (!pWidget->IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]))
					continue;

				if (!setFocus)
				{
					SetFocus(pWidget);
					setFocus = true;
				}

				bool handled = pWidget->Handle(msg);
				if (handled)
					break;
			}
		}
		break;

		case M_VirtualKeyDown:
		case M_VirtualKeyUp:
			if (m_pFocusedWidget)
				m_pFocusedWidget->Handle(msg);
			break;

		case M_CharKeyDown:
		{
			if (m_pFocusedWidget)
				m_pFocusedWidget->Handle(msg);
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
		while (pWidget->GetFocusPolicy() == Widget::FOCUS_POLICY::NO_FOCUS)
			pWidget = pWidget->GetParent();

		if (pWidget == m_pFocusedWidget)
			return;

		if (m_pFocusedWidget && m_pFocusedWidget->m_onLoseFocus)
			m_pFocusedWidget->m_onLoseFocus();

		if (pWidget && pWidget->m_onGetFocus)
			pWidget->m_onGetFocus();

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

	const std::string& WidgetMgr::GetEditorIconsPath() const
	{
		return m_editorIconsPath;
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
		m_pRoot->DeleteChild(m_pModalWindow);
		m_pModalWindow = nullptr;

		RequestResize();
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

	Message WidgetMgr::ConvertMessageToEvent(const Widget* pWidget, const Message& msg) const
	{
		switch (msg.m_id)
		{
		case M_MouseMove:
		{
				bool isInside = pWidget->IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]);
				bool wasInside = pWidget->IsInside(m_prevMouseX, m_prevMouseY);
				if (!wasInside && isInside)
				{
					Message ret;
					ret.m_id = MessageId::M_MouseEnter;
					ret.m_low = msg.m_low;
					return ret;
				}
				else if (wasInside && !isInside)
				{
					Message ret;
					ret.m_id = MessageId::M_MouseExit;
					ret.m_low = msg.m_low;
					return ret;
				}
				else if (isInside)
				{
					return msg;
				}
				else if (pWidget == m_pCapturedWidget) //the mouse is outside, only make anevnet if we captured the mouse
				{
					return msg;
				}
				else //this message doesn't concern this widget
				{
					Message ret;
					ret.m_id = MessageId::M_Invalid;
					return ret;
				}
		}
		break;

		case M_MouseLDown:
		case M_MouseLUp:
		case M_MouseMDown:
		case M_MouseMUp:
		case M_MouseRDown:
		case M_MouseRUp:
		{
			bool isInside = pWidget->IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]);
			bool isCaptured = m_pCapturedWidget == pWidget;

			if (isInside || isCaptured)
			{
				return msg;
			}
			else
			{
				Message ret;
				ret.m_id = MessageId::M_Invalid;
				return ret;
			}
		}
		break;

		case M_VirtualKeyDown:
		case M_VirtualKeyUp:
		case M_CharKeyDown:
		{
			if (m_pFocusedWidget == pWidget)
			{
				return msg;
			}
			else
			{
				Message ret;
				ret.m_id = MessageId::M_Invalid;
				return ret;
			}
		}
		break;

		default:
			assert(false);
		break;
		}

		Message ret;
		ret.m_id = MessageId::M_Invalid;
		return ret;
	}
}
