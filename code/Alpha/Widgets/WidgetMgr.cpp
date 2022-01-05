/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/WidgetMgr.h"

#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/Renderable/RenderableMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/ShaderMgr.h"


#include "Widgets/Message.h"
#include "Widgets/Widget.h"

#include "SysWindow.h"

#include <algorithm>

extern SysWindow* g_pWindow;

extern std::string g_dataRoot;
extern std::string g_shaderRoot;

WidgetMgr::WidgetMgr()
	: m_pRoot(nullptr)
	, m_prevMouseX(0)
	, m_prevMouseY(0)
	, m_pFocusedWidget(nullptr)
	, m_pCapturedWidget(nullptr)
{}

WidgetMgr::~WidgetMgr()
{
	if (m_pRoot)
		delete m_pRoot;
}

void WidgetMgr::Init()
{
	//Simple quad used to render widgets
	{
		VertexPosUv vertices[4] =
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

		Mesh* pSimpleQuad = nullptr;
		g_pMeshMgr->CreateMesh(&pSimpleQuad, m_quadMeshId);
		pSimpleQuad->LoadVertexAndIndexBuffer(vertices, _countof(vertices), indices, _countof(indices));
	}

	//Root signature for basic widget
	{
		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\widget.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\widget.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\widget.ps.cso");

		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(m_widgetPsoId, "widget");
		pPipelineState->Init_PosUv(rsId, vsId, psId);
	}

	//Renderable for basic widget
	{
		m_widgetRenderableId = g_pRenderableMgr->CreateRenderable(m_quadMeshId, m_widgetPsoId);
	}

	//Font for label
	{
		std::string fontFilename = g_dataRoot + "\\fonts\\segoeUI.fnt";
		Font* pFont = g_pFontMgr->CreateResource(m_segoeUIFontId, fontFilename);
		pFont->Init(fontFilename);

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\text.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\text.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\text.ps.cso");

		PipelineStateId text_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(text_pipelineStateId, "text");
		pPipelineState->Init_Text(rsId, vsId, psId);

		g_pRenderModule->InitialiseFont(m_segoeUIFontId, text_pipelineStateId, 1024);
	}

	//Renderable for icon
	{
		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\texture.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\texture.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\texture.ps.cso");

		PipelineStateId texture_posuv_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(texture_posuv_pipelineStateId, "texture");
		pPipelineState->Init_Icon(rsId, vsId, psId);

		m_iconRenderableId = g_pRenderableMgr->CreateRenderable(m_quadMeshId, texture_posuv_pipelineStateId);
	}
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
	if(m_pRoot && m_pRoot->IsEnabled())
		m_pRoot->Draw();
}

void WidgetMgr::Resize()
{
	if (!m_pRoot)
		return;

	DirectX::XMINT3 absPos(0, 0, 100);
	DirectX::XMUINT2 size(g_pWindow->GetWidth(), g_pWindow->GetHeight());
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
		for (std::deque<Widget*>::reverse_iterator it = m_sortedWidgets.rbegin(); it != m_sortedWidgets.rend(); ++it)
		{
			Widget* pWidget = *it;
			if (!pWidget->IsEnabled())
				continue;

			if (!pWidget->IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]))
				continue;

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
	m_pFocusedWidget = pWidget;
}

void WidgetMgr::CaptureMouse(Widget* pWidget)
{
	if (pWidget)
	{
		//You can have 2 widgets requesting to capture events
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

WidgetMgr* g_pWidgetMgr = nullptr;
