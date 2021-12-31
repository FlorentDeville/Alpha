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

#include "Window.h"

extern Window* g_pWindow;

WidgetMgr::WidgetMgr()
	: m_pRoot(nullptr)
	, m_prevMouseX(0)
	, m_prevMouseY(0)
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
		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature("C:\\workspace\\Alpha\\code\\x64\\Debug\\widget.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\widget.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\widget.ps.cso");

		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(m_widgetPsoId, "widget");
		pPipelineState->Init_PosUv(rsId, vsId, psId);
	}

	//Renderable for basic widget
	{
		m_widgetRenderableId = g_pRenderableMgr->CreateRenderable(m_quadMeshId, m_widgetPsoId);
	}

	//Font for label
	{
		std::string fontFilename = "C:\\workspace\\Alpha\\data\\fonts\\segoeUI.fnt";
		Font* pFont = g_pFontMgr->CreateResource(m_segoeUIFontId, fontFilename);
		pFont->Init(fontFilename);

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature("C:\\workspace\\Alpha\\code\\x64\\Debug\\text.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\text.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\text.ps.cso");

		PipelineStateId text_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(text_pipelineStateId, "text");
		pPipelineState->Init_Text(rsId, vsId, psId);

		g_pRenderModule->InitialiseFont(m_segoeUIFontId, text_pipelineStateId, 1024);
	}

	//Renderable for icon
	{
		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature("C:\\workspace\\Alpha\\code\\x64\\Debug\\texture.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\texture.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\texture.ps.cso");

		PipelineStateId texture_posuv_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(texture_posuv_pipelineStateId, "texture");
		pPipelineState->Init_Icon(rsId, vsId, psId);

		m_iconRenderableId = g_pRenderableMgr->CreateRenderable(m_quadMeshId, texture_posuv_pipelineStateId);
	}
}

void WidgetMgr::RegisterWidget(Widget* pWidget)
{
	m_widgets.push_back(pWidget);
}

void WidgetMgr::SetRoot(Widget* pRoot)
{
	m_pRoot = pRoot;
	RegisterWidget(pRoot);
}

void WidgetMgr::Draw()
{
	if(m_pRoot)
		m_pRoot->Draw();
}

void WidgetMgr::Resize()
{
	if (!m_pRoot)
		return;

	DirectX::XMINT3 absPos(0, 0, 100);
	DirectX::XMUINT2 size(g_pWindow->GetWidth(), g_pWindow->GetHeight());
	m_pRoot->Resize(absPos, size);
}

void WidgetMgr::HandleMsg(const Message& msg)
{
	switch (msg.m_id)
	{
	case M_MouseMove:
	{
		for (Widget* pWidget : m_widgets)
		{
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
	{
		for (Widget* pWidget : m_widgets)
		{
			if (!pWidget->IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]))
				continue;

			bool handled = pWidget->Handle(msg);
			if (handled)
				break;
		}
	}
		break;
	}
}

WidgetMgr* g_pWidgetMgr = nullptr;
