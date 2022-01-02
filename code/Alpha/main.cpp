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

#include "Rendering/CommandQueue.h"
#include "Helper.h"
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
#include "Widgets/Layout.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Message.h"
#include "Widgets/Split.h"
#include "Widgets/Split3Way.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Window.h"

#include "Resource/ResourceMgr.h"

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

TextureId g_textureId;
LPCWSTR g_pIconName = IDC_ARROW;

void LoadTexture()
{
	//std::string textureName = "C:\\workspace\\Alpha\\data\\textures\\grid_orange.png";
	std::string textureName = "C:\\workspace\\Alpha\\data\\fonts\\arial_0.tga";

	Texture* pTexture = g_pTextureMgr->CreateResource(g_textureId, textureName);
	pTexture->Init(textureName);
}

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

			case VK_F11:
				g_pWindow->ToggleFullscreen();
				break;

			case VK_NUMPAD0:
				g_perspectiveRendering = !g_perspectiveRendering;
				break;

			case VK_UP:
				//g_pButton->SetY(g_pButton->GetY() - 1);
				break;

			case VK_DOWN:
				//g_pButton->SetY(g_pButton->GetY() + 1);
				break;

			case VK_LEFT:
				//g_pButton->SetX(g_pButton->GetX() - 1);
				break;

			case VK_RIGHT:
				//g_pButton->SetX(g_pButton->GetX() + 1);
				break;

			default:
				return ::DefWindowProcW(hWnd, message, wParam, lParam);
				break;
			}
		}
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_SIZE:
	case WM_NCPAINT:
	{
		RECT clientRect = g_pWindow->GetClientRectangle();
		int width = clientRect.right - clientRect.left;
		int height = clientRect.bottom - clientRect.top;

		// Don't allow 0 size swap chain back buffers.
		uint32_t uwidth = std::max(1, width);
		uint32_t uheight = std::max(1, height);

		if (g_pWindow->GetWidth() != uwidth || g_pWindow->GetHeight() != uheight)
		{
			g_pWindow->Resize(uwidth, uheight);
			g_pRenderModule->ResizeSwapChain(uwidth, uheight);
			g_pRenderModule->ResizeDepthBuffer(uwidth, uheight);

			g_pRenderModule->m_viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(uwidth), static_cast<float>(uheight));

			g_pWidgetMgr->Resize();
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
		
		g_pWidgetMgr->HandleMsg(msg);
	}
		break;

	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseLDown;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		g_pWidgetMgr->HandleMsg(msg);
	}
	break;

	case WM_LBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseLUp;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		g_pWidgetMgr->HandleMsg(msg);
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
	// Update the model matrix.
	float angle = static_cast<float>(totalTimeElasped * 90.0);
	//float angle = 0;
	float scale = 2;
	g_model = DirectX::XMMatrixScaling(scale, scale, scale);

	const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));
	g_model = DirectX::XMMatrixMultiply(g_model, rot);

	// Update the view matrix.
	const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
	const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
	g_view = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

	// Update the projection matrix.
	float aspectRatio = g_pWindow->GetWidth() / static_cast<float>(g_pWindow->GetHeight());
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

	g_pWidgetMgr->Update();
}

void Render()
{
	g_pRenderModule->PreRender();

	// Render the cube
	//if(false)
	{
		//// Update the MVP matrix
		DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
		mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, g_projection);
		g_pRenderModule->Render(*g_pRenderableMgr->GetRenderable(g_CubeId), mvpMatrix);
	}

	//Render texture cube
	if(false)
	{
		const Renderable* renderable = g_pRenderableMgr->GetRenderable(g_CubeTextureId);
		g_pRenderModule->PreRenderForRenderable(*renderable);

		//DirectX::XMMATRIX wvpMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), g_view);
		DirectX::XMMATRIX wvpMatrix = DirectX::XMMatrixMultiply(g_model, g_view);
		wvpMatrix = DirectX::XMMatrixMultiply(wvpMatrix, g_projection);

		g_pRenderModule->SetConstantBuffer(0, sizeof(wvpMatrix), &wvpMatrix, 0);

		ID3D12DescriptorHeap* pSrv = g_pTextureMgr->GetResource(g_textureId)->GetSRV();
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		g_pRenderModule->GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		g_pRenderModule->GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());

		g_pRenderModule->PostRenderForRenderable(*renderable);
	}
	

	// Render the widgets
	//if(false)
	{
		g_pWidgetMgr->Draw();
	}

	//render text
	//if(false)
	{
		//g_pRenderModule->PrepareRenderText("Hello World", g_segoeUIFontId, DirectX::XMFLOAT3(0, 0, 2), DirectX::XMFLOAT2(1, 1));
		g_pRenderModule->PrepareRenderText("Hello World", g_comicSansMsFontId, DirectX::XMFLOAT3(540, 420, 2), DirectX::XMFLOAT2(3, 3));
	}

	g_pRenderModule->RenderAllText();
	g_pRenderModule->PostRender();
}

bool LoadContent()
{
	{
		Mesh* pCubeMesh = nullptr;
		MeshId cubeMeshId;
		g_pMeshMgr->CreateMesh(&pCubeMesh, cubeMeshId);
		pCubeMesh->LoadVertexAndIndexBuffer(g_Vertices, _countof(g_Vertices), g_Indicies, _countof(g_Indicies));

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature("C:\\workspace\\Alpha\\code\\x64\\Debug\\base.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\base.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\base.ps.cso");

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

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature("C:\\workspace\\Alpha\\code\\x64\\Debug\\texture.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\texture.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\texture.ps.cso");

		PipelineStateId texture_posuv_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(texture_posuv_pipelineStateId, "texture");
		pPipelineState->Init_PosUv(rsId, vsId, psId);

		g_CubeTextureId = g_pRenderableMgr->CreateRenderable(cubeTextureMeshId, texture_posuv_pipelineStateId);
	}

	//Load the font
	{
		{
			std::string fontFilename = "C:\\workspace\\Alpha\\data\\fonts\\comicSansMs.fnt";
			Font* pFont = g_pFontMgr->CreateResource(g_comicSansMsFontId, fontFilename);
			pFont->Init(fontFilename);
		}

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature("C:\\workspace\\Alpha\\code\\x64\\Debug\\text.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\text.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\text.ps.cso");

		PipelineStateId text_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(text_pipelineStateId, "text");
		pPipelineState->Init_Text(rsId, vsId, psId);

		g_pRenderModule->InitialiseFont(g_comicSansMsFontId, text_pipelineStateId, 1024);

	}

	g_contentLoaded = true;

	return true;
}

void CreateMenuBar()
{
	Widgets::Window* pWindow = new Widgets::Window(DirectX::XMUINT2(g_pWindow->GetWidth(), g_pWindow->GetHeight()));
	g_pWidgetMgr->SetRoot(pWindow);
	g_pWidgetMgr->Resize();

	Widgets::Split3Way* pSplit = new Widgets::Split3Way();
	pSplit->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	pWindow->AddWidget(pSplit);
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	g_pFontMgr = new RESOURCE_MGR(Font);
	g_pFontMgr->Init();

	g_pPipelineStateMgr = new RESOURCE_MGR(PipelineState);
	g_pPipelineStateMgr->Init();

	g_pTextureMgr = new RESOURCE_MGR(Texture);
	g_pTextureMgr->Init();

	int width = 1080;
	int height = 789;

	//width = GetSystemMetrics(SM_CXFULLSCREEN);
	//height = GetSystemMetrics(SM_CYFULLSCREEN);

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	const wchar_t* pWindowClassName = L"DX12WindowClass";
	SysWindow::RegisterWindowClass(hInstance, pWindowClassName, WndProc);
	g_pWindow = new SysWindow();
	g_pWindow->Create(pWindowClassName, L"Alpha", width, height, hInstance);

	g_pRenderModule = new RenderModule();
	g_pRenderModule->Init(g_pWindow->GetWindowHandle(), width, height);

	g_pMeshMgr = new MeshMgr();
	g_pRootSignatureMgr = new RootSignatureMgr();
	g_pShaderMgr = new ShaderMgr();
	g_pRenderableMgr = new RenderableMgr();
	g_pWidgetMgr = new WidgetMgr();
	g_pWidgetMgr->Init();

	g_IsInitialized = true;
	g_contentLoaded = false;
	LoadContent();

	LoadTexture();

	CreateMenuBar();

	HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
	SetCursor(hCurs1);

	g_pWindow->Show();

	{
		g_pRenderModule->m_scissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
		g_pRenderModule->m_viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));
		g_FoV = 45.f;
	}

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	g_pRenderModule->Shutdown();
	g_pTextureMgr->Release();
	g_pPipelineStateMgr->Release();
	g_pFontMgr->Release();

	delete g_pFontMgr;
	delete g_pTextureMgr;
	delete g_pWidgetMgr;
	delete g_pRenderableMgr;
	delete g_pShaderMgr;
	delete g_pRootSignatureMgr;
	delete g_pPipelineStateMgr;
	delete g_pMeshMgr;
	delete g_pWindow;
	delete g_pRenderModule;

	return 0;
}
