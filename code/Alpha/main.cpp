/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define NOMINMAX

#include <assert.h>
#include <cstdint>
#include <exception>
#include <chrono>
#include <windowsx.h>

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// D3D12 extension library.
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

#include "Window.h"
#include "Widgets/Button.h"
#include "Widgets/HLayout.h"
#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

#include "Resource/ResourceMgr.h"

bool g_VSync;

bool g_IsInitialized = false;

Window* g_pWindow = nullptr;
HLayout* g_pButton = nullptr;

RenderableId g_CubeId;
RenderableId g_SimpleQuadId;
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

ID3D12DescriptorHeap* g_pSrvDescriptorHeap = nullptr;

FontId g_segoeUIFontId;
FontId g_comicSansMsFontId;

void LoadTexture()
{
	//std::string textureName = "C:\\workspace\\Alpha\\data\\textures\\grid_orange.png";
	std::string textureName = "C:\\workspace\\Alpha\\data\\fonts\\arial_0.tga";

	TextureId id;
	Texture* pTexture = g_pTextureMgr->CreateResource(id, textureName);
	pTexture->Init(textureName);

	ID3D12Device* pDevice = g_pRenderModule->GetDevice();

	//Create the SRV heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		HRESULT res = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&g_pSrvDescriptorHeap));
		ThrowIfFailed(res);
	}

	//Create the srv descriptor
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = pTexture->GetResourceDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		pDevice->CreateShaderResourceView(pTexture->GetResource(), &srvDesc, g_pSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}
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
				g_pButton->SetY(g_pButton->GetY() - 1);
				break;

			case VK_DOWN:
				g_pButton->SetY(g_pButton->GetY() + 1);
				break;

			case VK_LEFT:
				g_pButton->SetX(g_pButton->GetX() - 1);
				break;

			case VK_RIGHT:
				g_pButton->SetX(g_pButton->GetX() + 1);
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
	{
		RECT clientRect = g_pWindow->GetWindowRectangle();
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
		}
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

		g_pWidgetMgr->HandleMsg(msg);
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
		OutputDebugString(buffer);

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
}

void Render()
{
	g_pRenderModule->PreRender();

	// Render the cube
	if(false)
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

		ID3D12DescriptorHeap* pDescriptorHeap[] = { g_pSrvDescriptorHeap };
		g_pRenderModule->GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		g_pRenderModule->GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, g_pSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		g_pRenderModule->PostRenderForRenderable(*renderable);
	}
	

	// Render the widgets
	//if(false)
	{
		g_pWidgetMgr->Draw();
	}

	// Render simple quad
	if(false)
	{
		float width = 100;//static_cast<float>(m_width);
		float height = 100;//static_cast<float>(m_height);
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, height, 0);

		//position : top left corner
		float windowWidth = static_cast<float>(g_pWindow->GetWidth());
		float windowHeight = static_cast<float>(g_pWindow->GetHeight());

		float x = 50 + width * 0.5f;
		float y = 50 - height * 0.5f;
		float z = 1;
		DirectX::XMMATRIX position = DirectX::XMMatrixTranslation(x, y, z);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 0, 10, 1), DirectX::XMVectorSet(0, 1, 0, 1));

		float projWidth = static_cast<float>(windowWidth);
		float projHeight = static_cast<float>(windowHeight);
		DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(projWidth, projHeight, 0.1f, 100.f);

		DirectX::XMMATRIX wvp = DirectX::XMMatrixMultiply(scale, position);
		wvp = DirectX::XMMatrixMultiply(wvp, view);
		wvp = DirectX::XMMatrixMultiply(wvp, projection);

		DirectX::XMVECTOR color = DirectX::XMVectorSet(0.12f, 0.12f, 0.12f, 1);

		const Renderable* renderable = g_pRenderableMgr->GetRenderable(g_SimpleQuadId);
		g_pRenderModule->PreRenderForRenderable(*renderable);
		g_pRenderModule->SetConstantBuffer(0, sizeof(wvp), &wvp, 0);
		g_pRenderModule->SetConstantBuffer(1, sizeof(color), &color, 0);
		int showBorder = 0;
		g_pRenderModule->SetConstantBuffer(2, sizeof(showBorder), &showBorder, 0);
		g_pRenderModule->PostRenderForRenderable(*renderable);
	}

	//render text
	//if(false)
	{
		g_pRenderModule->PrepareRenderText("Hello World", g_segoeUIFontId, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1, 1));
		g_pRenderModule->PrepareRenderText("The Lord Of The Rings", g_comicSansMsFontId, DirectX::XMFLOAT2(540, 420), DirectX::XMFLOAT2(3, 3));
		g_pRenderModule->RenderAllText();
	}

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

	//Load the simple quad used by the widgets
	{
		Mesh* pSimpleQuad = nullptr;
		MeshId simpleQuadMeshId;
		g_pMeshMgr->CreateMesh(&pSimpleQuad, simpleQuadMeshId);
		pSimpleQuad->LoadVertexAndIndexBuffer(g_SimpleQuad, _countof(g_SimpleQuad), g_QuadIndices, _countof(g_QuadIndices));

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature("C:\\workspace\\Alpha\\code\\x64\\Debug\\widget.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\widget.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader("C:\\workspace\\Alpha\\code\\x64\\Debug\\widget.ps.cso");

		PipelineStateId widget_Pos_pipelineStateId;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(widget_Pos_pipelineStateId, "widget");
		pPipelineState->Init_PosUv(rsId, vsId, psId);

		g_SimpleQuadId = g_pRenderableMgr->CreateRenderable(simpleQuadMeshId, widget_Pos_pipelineStateId);
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
			std::string fontFilename = "C:\\workspace\\Alpha\\data\\fonts\\segoeUI.fnt";
			Font* pFont = g_pFontMgr->CreateResource(g_segoeUIFontId, fontFilename);
			pFont->Init(fontFilename);
		}

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

		g_pRenderModule->InitialiseFont(g_segoeUIFontId, text_pipelineStateId, 1024);
		g_pRenderModule->InitialiseFont(g_comicSansMsFontId, text_pipelineStateId, 1024);

	}

	g_contentLoaded = true;

	return true;
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
	Window::RegisterWindowClass(hInstance, pWindowClassName, WndProc);
	g_pWindow = new Window();
	g_pWindow->Create(pWindowClassName, L"Alpha", width, height, hInstance);

	g_pRenderModule = new RenderModule();
	g_pRenderModule->Init(g_pWindow->GetWindowHandle(), width, height);

	g_pMeshMgr = new MeshMgr();
	g_pRootSignatureMgr = new RootSignatureMgr();
	g_pShaderMgr = new ShaderMgr();
	g_pRenderableMgr = new RenderableMgr();
	g_pWidgetMgr = new WidgetMgr();

	g_IsInitialized = true;
	g_contentLoaded = false;
	LoadContent();

	g_pButton = new HLayout(1000, 200, 0, 0);
	g_pButton->SetBackgroundColor(DirectX::XMVectorSet(1.f, 0.f, 0.f, 1.f));
	g_pButton->AddWidget(new Button(100, 50, 0, 0));
	g_pButton->AddWidget(new Button(300, 100, 0, 0));
	g_pButton->AddWidget(new Button(400, 150, 0, 0));
	g_pButton->Resize(DirectX::XMINT3(0, 0, 100), DirectX::XMUINT2(width, height));
	g_pWidgetMgr->SetRoot(g_pButton);

	LoadTexture();

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
