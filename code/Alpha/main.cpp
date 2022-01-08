/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define NOMINMAX

#include <assert.h>
#include <cstdint>
#include <exception>
#include <chrono>
#include <windowsx.h>


#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "Core/CommandLine.h"
#include "Core/Helper.h"

#include "Rendering/CommandQueue.h"
#include "Rendering/Font/Font.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Renderable/RenderableMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

#include "SysWindow.h"
#include "Widgets/Button.h"
#include "Widgets/Container.h"
#include "Widgets/Layout.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Message.h"
#include "Widgets/Split.h"
#include "Widgets/Split3Way.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Window.h"

#include "Core/Resource/ResourceMgr.h"

bool g_VSync;

bool g_IsInitialized = false;

SysWindow* g_pWindow = nullptr;

RenderableId g_CubeId;
RenderableId g_CubeTextureId;

float g_FoV;

DirectX::XMMATRIX g_model;
DirectX::XMMATRIX g_view;
DirectX::XMMATRIX g_projection;

bool g_contentLoaded;

bool g_perspectiveRendering = true;

float g_posX = 0;
float g_posY = 0;

static VertexPosColor g_Vertices[8] = {
	{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
	{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f),	DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
	{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f),	DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
	{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),	DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
	{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
	{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f),	DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
	{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f),	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
	{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f),	DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
};

static uint16_t g_Indicies[36] =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};

static VertexPosUv g_SimpleQuad[4] =
{
	{ DirectX::XMFLOAT3(-0.5f, 0.5f , 0.f), DirectX::XMFLOAT2(0.f, 0.f) },	// top left
	{ DirectX::XMFLOAT3(0.5f , 0.5f , 0.f),	DirectX::XMFLOAT2(1.f, 0.f) },	// top right
	{ DirectX::XMFLOAT3(0.5f , -0.5f, 0.f),	DirectX::XMFLOAT2(1.f, 1.f) },	// bottom right
	{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.f),	DirectX::XMFLOAT2(0.f, 1.f) }	// bottom left
};

static uint16_t g_QuadIndices[6]
{
	0, 1, 2,
	2, 3, 0
};

static VertexPosUv g_CubeTexture[] = {
	// front face
	{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2( 0.0f, 0.0f) },
	{ DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2( 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2( 0.0f, 1.0f) },
	{ DirectX::XMFLOAT3( 0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2( 1.0f, 0.0f) },
	  
	//DirectX::XMFLOAT3( right side face   ), DirectX::XMFLOAT2(		   )
	{ DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2( 0.0f, 1.0f) },
	{ DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2( 1.0f, 0.0f) },
	{ DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2( 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3( 0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2( 0.0f, 0.0f) },
	  
	//DirectX::XMFLOAT3( left side face	   ), DirectX::XMFLOAT2(		   )
	{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2( 0.0f, 0.0f) },
	{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2( 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2( 0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2( 1.0f, 0.0f) },
	  
	//DirectX::XMFLOAT3( back face		   ), DirectX::XMFLOAT2(		   )
	{ DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2( 0.0f, 0.0f) },
	{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2( 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2( 0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2( 1.0f, 0.0f) },
	  
	//DirectX::XMFLOAT3( top face		   ), DirectX::XMFLOAT2(		   )
	{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2( 0.0f, 1.0f) },
	{ DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2( 1.0f, 0.0f) },
	{ DirectX::XMFLOAT3( 0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2( 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2( 0.0f, 0.0f) },
	  
	//DirectX::XMFLOAT3( bottom face	   ), DirectX::XMFLOAT2(		   )
	{ DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2( 0.0f, 0.0f) },
	{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2( 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2( 0.0f, 1.0f) },
	{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2( 1.0f, 0.0f) },
};

static uint16_t g_CubeTextureIndices[] =
{
	0, 1, 2,
	0, 3, 1/*,

	4, 5, 7,
	4, 6, 5,

	8, 9, 10,
	10, 11, 9,

	12, 13, 14,
	14, 15, 12,

	16, 17, 18,
	18, 19, 16,

	20, 21, 22,
	22, 23, 20*/
};

void Update();
void Render();

FontId g_comicSansMsFontId;

PipelineStateId g_texture_posuv_pipelineStateId;

TextureId g_textureId;
LPCWSTR g_pIconName = IDC_ARROW;

std::string g_dataRoot;
std::string g_shaderRoot;

void LoadTexture()
{
	//std::string textureName = "C:\\workspace\\Alpha\\data\\textures\\grid_orange.png";
	std::string textureName = g_dataRoot + "\\fonts\\arial_0.tga";

	Texture* pTexture = g_pTextureMgr->CreateResource(g_textureId, textureName);
	pTexture->Init(textureName);
}

DirectX::XMVECTOR g_eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
//DirectX::XMVECTOR g_focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
DirectX::XMVECTOR g_direction = DirectX::XMVectorSet(0, 0, 1, 1);
DirectX::XMVECTOR g_upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
DirectX::XMVECTOR g_euler = DirectX::XMVectorSet(0, 0, 0, 1);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!g_IsInitialized)
		return ::DefWindowProcW(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_PAINT:
		Update();
		Render();
		break;

	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;

			default:
			{
				Message msg;
				msg.m_id = M_KeyDown;
				msg.m_high = wParam;
				WidgetMgr::Get().HandleMsg(msg);
			}
			break;
			}
		}
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
			return true;

		RECT clientRect = g_pWindow->GetClientRectangle();
		int width = clientRect.right - clientRect.left;
		int height = clientRect.bottom - clientRect.top;

		// Don't allow 0 size swap chain back buffers.
		uint32_t uwidth = std::max(1, width);
		uint32_t uheight = std::max(1, height);

		if (g_pWindow->GetWidth() != uwidth || g_pWindow->GetHeight() != uheight)
		{
			g_pWindow->Resize(uwidth, uheight);
			RenderModule::Get().ChangeMainResolution(DirectX::XMUINT2(uwidth, uheight));
			WidgetMgr::Get().Resize();
		}

		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	}
		break;

	case WM_NCPAINT:
	{
		RECT clientRect = g_pWindow->GetClientRectangle();
		int width = clientRect.right - clientRect.left;
		int height = clientRect.bottom - clientRect.top;

		//This is a minimiz event. Ignore it.
		if (width == 0 && height == 0)
			return true;

		// Don't allow 0 size swap chain back buffers.
		uint32_t uwidth = std::max(1, width);
		uint32_t uheight = std::max(1, height);

		if (g_pWindow->GetWidth() != uwidth || g_pWindow->GetHeight() != uheight)
		{
			g_pWindow->Resize(uwidth, uheight);
			RenderModule::Get().ChangeMainResolution(DirectX::XMUINT2(uwidth, uheight));
			WidgetMgr::Get().Resize();
		}

		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	}
		break;

	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseMove;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		switch (wParam)
		{
			case MK_LBUTTON:
				msg.m_high = M_LButton;
				break;

			default:
				msg.m_high = M_None;
				break;
		}
		
		WidgetMgr::Get().HandleMsg(msg);
	}
		break;

	case WM_LBUTTONDOWN:
	{
		SetCapture(hWnd);

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseLDown;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		WidgetMgr::Get().HandleMsg(msg);
	}
	break;

	case WM_LBUTTONUP:
	{
		ReleaseCapture();

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseLUp;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		WidgetMgr::Get().HandleMsg(msg);
	}

	case WM_SETCURSOR:
	{
		HCURSOR hCurs1 = LoadCursor(NULL, g_pIconName);
		SetCursor(hCurs1);
		return 1;
	}
		break;

	default:
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void Update()
{
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();
	static auto start = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	if (elapsedSeconds > 1.0)
	{
		wchar_t buffer[500];
		auto fps = frameCounter / elapsedSeconds;
		swprintf_s(buffer, 500, L"FPS: %f\n", fps);
		//OutputDebugString(buffer);

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}

	auto dt = clock.now() - start;
	double totalTimeElasped = dt.count() * 1e-9;
	totalTimeElasped;
	// Update the model matrix.
	//float angle = static_cast<float>(totalTimeElasped * 90.0);
	float angle = 0;
	float scale = 0.5;
	g_model = DirectX::XMMatrixScaling(scale, scale, scale);

	const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));
	g_model = DirectX::XMMatrixMultiply(g_model, rot);

	// Update the view matrix.
	DirectX::XMMATRIX orientation = DirectX::XMMatrixRotationRollPitchYawFromVector(g_euler);
	DirectX::XMVECTOR direction = DirectX::XMVector3Transform(g_direction, orientation);
	g_view = DirectX::XMMatrixLookToLH(g_eyePosition, direction, g_upDirection);

	// Update the projection matrix.
	const DirectX::XMUINT2 gameResolution = RenderModule::Get().GetGameResolution();
	float aspectRatio = gameResolution.x / static_cast<float>(gameResolution.y);
	if (g_perspectiveRendering)
	{				
		float nearDistance = 0.1f;
		float fovRad = DirectX::XMConvertToRadians(g_FoV);
		g_projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearDistance, 100.0f);
	}
	else
	{
		float w = 20.f;
		float h = w / aspectRatio;
		g_projection = DirectX::XMMatrixOrthographicLH(w, h, 0.1f, 100.f);
	}

	WidgetMgr::Get().Update();
}

void Render()
{
	RenderModule::Get().PreRender_RenderToTexture();

	// Render the cube
	//if(false)
	{
		//// Update the MVP matrix
		DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
		mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, g_projection);
		RenderModule::Get().Render(*g_pRenderableMgr->GetRenderable(g_CubeId), mvpMatrix);
	}

	RenderModule::Get().PreRender();

	// Render the cube
	//if(false)
	//{
	//	//// Update the MVP matrix
	//	DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
	//	mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, g_projection);
	//	g_pRenderModule->Render(*g_pRenderableMgr->GetRenderable(g_CubeId), mvpMatrix);
	//}

	//Render texture cube
	if(false)
	{
		const Renderable* renderable = g_pRenderableMgr->GetRenderable(g_CubeTextureId);
		RenderModule::Get().PreRenderForRenderable(*renderable);

		//DirectX::XMMATRIX wvpMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), g_view);
		DirectX::XMMATRIX wvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
		wvpMatrix = DirectX::XMMatrixMultiply(wvpMatrix, g_projection);

		RenderModule::Get().SetConstantBuffer(0, sizeof(wvpMatrix), &wvpMatrix, 0);

		ID3D12DescriptorHeap* pSrv = g_pTextureMgr->GetResource(g_textureId)->GetSRV();
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		RenderModule::Get().GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		RenderModule::Get().GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());

		RenderModule::Get().PostRenderForRenderable(*renderable);
	}
	

	// Render the widgets
	//if(false)
	{
		WidgetMgr::Get().Draw();
	}

	RenderModule::Get().RenderAllText();
	RenderModule::Get().PostRender();
}

bool LoadContent()
{
	{
		Mesh* pCubeMesh = nullptr;
		MeshId cubeMeshId;
		g_pMeshMgr->CreateMesh(&pCubeMesh, cubeMeshId);
		pCubeMesh->LoadVertexAndIndexBuffer(g_Vertices, _countof(g_Vertices), g_Indicies, _countof(g_Indicies));

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\base.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.ps.cso");

		PipelineStateId base_PosColor_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(base_PosColor_pipelineStateId, "base");
		pPipelineState->Init_PosColor(rsId, vsId, psId);

		g_CubeId = g_pRenderableMgr->CreateRenderable(cubeMeshId, base_PosColor_pipelineStateId);
	}

	//Load the textured cube
	{
		Mesh* pCubeTexture = nullptr;
		MeshId cubeTextureMeshId;
		g_pMeshMgr->CreateMesh(&pCubeTexture, cubeTextureMeshId);
		pCubeTexture->LoadVertexAndIndexBuffer(g_CubeTexture, _countof(g_CubeTexture), g_CubeTextureIndices, _countof(g_CubeTextureIndices));

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\texture.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\texture.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\texture.ps.cso");

		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(g_texture_posuv_pipelineStateId, "texture");
		pPipelineState->Init_PosUv(rsId, vsId, psId);

		g_CubeTextureId = g_pRenderableMgr->CreateRenderable(cubeTextureMeshId, g_texture_posuv_pipelineStateId);
	}

	//Load the font
	{
		{
			std::string fontFilename = g_dataRoot + "\\fonts\\comicSansMs.fnt";
			Font* pFont = RenderModule::Get().GetFontMgr().CreateResource(g_comicSansMsFontId, fontFilename);
			pFont->Init(fontFilename);
		}

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\text.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\text.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\text.ps.cso");

		PipelineStateId text_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(text_pipelineStateId, "text");
		pPipelineState->Init_Text(rsId, vsId, psId);

		RenderModule::Get().InitialiseFont(g_comicSansMsFontId, text_pipelineStateId, 1024);

	}

	g_contentLoaded = true;

	return true;
}

void CreateMainWindow()
{
	Widgets::Window* pWindow = new Widgets::Window(DirectX::XMUINT2(g_pWindow->GetWidth(), g_pWindow->GetHeight()));
	WidgetMgr::Get().SetRoot(pWindow);

	Widgets::Split3Way* pSplit = new Widgets::Split3Way();
	pSplit->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	pWindow->AddWidget(pSplit);

	Widgets::TabContainer* pMiddleTabContainer = new Widgets::TabContainer();
	pSplit->AddMiddlePanel(pMiddleTabContainer);

	Widgets::Tab* pDummyTab1 = new Widgets::Tab();
	pMiddleTabContainer->AddTab("Dumb", pDummyTab1);

	Widgets::Tab* pViewportTab = new Widgets::Tab();
	pMiddleTabContainer->AddTab("Game", pViewportTab);

	Widgets::Tab* pDummyTab2 = new Widgets::Tab();
	pMiddleTabContainer->AddTab("Dumber", pDummyTab2);

	pMiddleTabContainer->SetSelectedTab(1);

	Widgets::Viewport* pViewport = new Widgets::Viewport();
	pViewport->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	pViewportTab->AddWidget(pViewport);

	WidgetMgr::Get().Resize();
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR lpCmdLine, _In_ int /*nCmdShow*/)
{
	CommandLine cmd;
	cmd.AddArgument("-dataroot", "", CommandLine::ARG_TYPE::STRING, CommandLine::ARG_ACTION::STORE, &g_dataRoot, "Path of the data folder.");
	cmd.Parse(lpCmdLine);

	const int size = 256;
	char buffer[size];
	GetModuleFileNameA(NULL, buffer, size);
	g_shaderRoot = buffer;
	size_t lastSlash = g_shaderRoot.find_last_of('\\');
	g_shaderRoot = g_shaderRoot.substr(0, lastSlash);

	g_pPipelineStateMgr = new RESOURCE_MGR(PipelineState);
	g_pPipelineStateMgr->Init();

	g_pTextureMgr = new RESOURCE_MGR(Texture);
	g_pTextureMgr->Init();

	DirectX::XMUINT2 windowResolution(1080, 789);
	DirectX::XMUINT2 gameResolution = windowResolution;
	
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	const wchar_t* pWindowClassName = L"DX12WindowClass";
	SysWindow::RegisterWindowClass(hInstance, pWindowClassName, WndProc);
	g_pWindow = new SysWindow();
	g_pWindow->Create(pWindowClassName, L"Alpha", windowResolution.x, windowResolution.y, hInstance);

	RenderModule& render = RenderModule::InitSingleton();
	render.Init(g_pWindow->GetWindowHandle(), gameResolution, windowResolution);

	g_pMeshMgr = new MeshMgr();
	g_pRootSignatureMgr = new RootSignatureMgr();
	g_pShaderMgr = new ShaderMgr();
	g_pRenderableMgr = new RenderableMgr();

	WidgetMgr& widgetMgr = WidgetMgr::InitSingleton();
	widgetMgr.Init();

	g_IsInitialized = true;
	g_contentLoaded = false;
	LoadContent();

	LoadTexture();

	CreateMainWindow();

	HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
	SetCursor(hCurs1);

	g_pWindow->Show();

	g_FoV = 45.f;

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	render.Release();
	RenderModule::ReleaseSingleton();

	g_pTextureMgr->Release();
	g_pPipelineStateMgr->Release();

	WidgetMgr::Get().Release();
	WidgetMgr::ReleaseSingleton();

	delete g_pTextureMgr;
	delete g_pRenderableMgr;
	delete g_pShaderMgr;
	delete g_pRootSignatureMgr;
	delete g_pPipelineStateMgr;
	delete g_pMeshMgr;
	delete g_pWindow;

	return 0;
}
