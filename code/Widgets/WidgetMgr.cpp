/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/WidgetMgr.h"

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

WidgetMgr::WidgetMgr()
	: m_pRoot(nullptr)
	, m_prevMouseX(0)
	, m_prevMouseY(0)
	, m_pFocusedWidget(nullptr)
	, m_pCapturedWidget(nullptr)
	, m_widgetViewportPsoId()
	, m_segoeUIFontId()
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
		RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\widget.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\widget.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\widget.ps.cso");

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
		Font* pFont = RenderModule::Get().GetFontMgr().CreateResource(m_segoeUIFontId, fontName);
		pFont->Init(parameter.m_dataFontsPath, fontName);

		RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\text.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\text.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\text.ps.cso");

		Rendering::PipelineStateId text_pipelineStateId;
		Rendering::PipelineState* pPipelineState = pipelineStateMgr.CreatePipelineState(text_pipelineStateId);
		pPipelineState->Init_Text(rsId, vsId, psId);

		RenderModule::Get().InitialiseFont(m_segoeUIFontId, text_pipelineStateId, 65535);
	}

	//material for icon
	{
		RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\texture.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\texture.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\texture.ps.cso");

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
		RootSignatureId rsId = rootSignatureMgr.CreateRootSignature(parameter.m_gameShaderPath + "\\widget_viewport.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\widget_viewport.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(parameter.m_gameShaderPath + "\\widget_viewport.ps.cso");

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

void WidgetMgr::SetRoot(Widget* pRoot)
{
	m_pRoot = pRoot;
	RegisterWidget(pRoot);
}

void WidgetMgr::Update()
{
	for (Widget* pWidget : m_widgets)
		pWidget->Update();
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

	switch (msg.m_id)
	{
	case M_MouseMove:
	{
		for(std::deque<Widget*>::reverse_iterator it = m_sortedWidgets.rbegin(); it != m_sortedWidgets.rend(); ++it)
		{
			Widget* pWidget = *it;
			if (!pWidget->IsEnabled())
				continue;

			bool handled = false;
			if(pWidget->IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]))
			{
				if (!pWidget->IsInside(m_prevMouseX, m_prevMouseY)) //if the previous position was outside, send a Entering message
				{
					Message enteringMsg;
					enteringMsg.m_id = M_MouseEnter;
					enteringMsg.m_low.m_raw = msg.m_low.m_raw;

					handled = pWidget->Handle(enteringMsg);
				}
				else
				{
					handled = pWidget->Handle(msg);
				}
			}
			else if (pWidget->IsInside(m_prevMouseX, m_prevMouseY)) //if the previous pos was inside, send a exit message
			{
				Message exitMsg;
				exitMsg.m_id = M_MouseExit;
				exitMsg.m_low.m_raw = msg.m_low.m_raw;

				handled = pWidget->Handle(exitMsg);
			}

			//I can't break here, if I entered a widget, I exited another so I need to loop through everything
			//Once I have a proper way of checking what widget are where, I won't need to iterate like this.
			/*if (handled)
				break;*/
		}

		m_prevMouseX = msg.m_low.m_pos[0];
		m_prevMouseY = msg.m_low.m_pos[1];
	}
		break;

	case M_MouseLDown:
	case M_MouseLUp:
	{
		bool setFocus = false;
		for (std::deque<Widget*>::reverse_iterator it = m_sortedWidgets.rbegin(); it != m_sortedWidgets.rend(); ++it)
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

	case M_KeyDown:
		if (m_pFocusedWidget)
			m_pFocusedWidget->Handle(msg);
		break;
	}
}

FontId WidgetMgr::GetUIFontId() const
{
	return m_segoeUIFontId;
}

DirectX::XMINT2 WidgetMgr::GetCursorPosition() const
{
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);

	return DirectX::XMINT2 (cursorPosition.x, cursorPosition.y);
}

void WidgetMgr::SetFocus(Widget* pWidget)
{
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

void WidgetMgr::ComputeSortedWidgetQueue()
{
	m_sortedWidgets.clear();

	for (Widget* pWidget : m_widgets)
		m_sortedWidgets.push_back(pWidget);

	std::sort(m_sortedWidgets.begin(), m_sortedWidgets.end(), [](Widget* pA, Widget* pB) { return pA->m_absPos.z > pB->m_absPos.z; });
}

const Widget* WidgetMgr::GetFocusedWidget() const
{
	return m_pFocusedWidget;
}
