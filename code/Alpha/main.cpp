/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define NOMINMAX

#include <assert.h>
#include <cmath>
#include <cstdint>
#include <exception>
#include <chrono>
#include <windowsx.h>

#include <DirectXMath.h>

#include "Core/CommandLine.h"
#include "Core/Helper.h"

#include "Editors/GamePlayer/GamePlayer.h"
#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/MeshEditor/MeshEditor.h"
#include "Editors/ShaderEditor/ShaderEditor.h"
#include "Editors/Widgets/List/Models/AssetListModel.h"

#include "Inputs/InputMgr.h"

#include "Configuration.h"
#include "GameMgr.h"

#include "Rendering/CommandQueue.h"
#include "Rendering/Font/Font.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Material/Material.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Resources/ResourcesMgr.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Loader.h"

#include "Systems/Reflection/ReflectionCoreTypes.h"
#include "Systems/Reflection/ReflectionMgr.h"
#include "Systems/Reflection/ReflectionStandardTypes.h"
#include "Systems/Reflection/ReflectionSystemsTypes.h"

#include "OsWin/SysWindow.h"

#include "Widgets/Button.h"
#include "Widgets/Container.h"
#include "Widgets/Layout.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Message.h"
#include "Widgets/Split.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/TextBox.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/TableView.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // DEBUG


//#pragma optimize("", off)

SysWindow* g_pWindow = nullptr;

LPCSTR g_pIconName = IDC_ARROW;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
	{
		Message msg;
		msg.m_id = M_CharKeyDown;
		msg.m_high = wParam;
		Widgets::WidgetMgr::Get().HandleMsg(msg);
	}
	break;

	case WM_KEYDOWN:
	{
		//wParam is a virtual key code
		Message msg;
		msg.m_id = M_VirtualKeyDown;
		msg.m_high = wParam; 
		Widgets::WidgetMgr::Get().HandleMsg(msg);
		Inputs::InputMgr::Get().UpdateKeyboardState(wParam, true);
	}
	break;

	case WM_KEYUP:
	{
		//wParam is a virtual key code
		Message msg;
		msg.m_id = M_VirtualKeyUp;
		msg.m_high = wParam;
		Widgets::WidgetMgr::Get().HandleMsg(msg);
		Inputs::InputMgr::Get().UpdateKeyboardState(wParam, false);
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
			DirectX::XMUINT2 size(uwidth, uheight);
			g_pWindow->Resize(uwidth, uheight);
			Rendering::RenderModule::Get().ChangeMainResolution(size);
			Widgets::WidgetMgr::Get().Resize();
		}

		return ::DefWindowProc(hWnd, message, wParam, lParam);
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
	
		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
		inputMgr.UpdateMouseState(mouseState);
	}
		break;

	case WM_MOUSEWHEEL:
	{
		int16_t wheelDistance = GET_WHEEL_DELTA_WPARAM(wParam);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();

		mouseState.m_mouseWheel = wheelDistance;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);

	}
		break;

	case WM_MBUTTONDOWN:
	{
		uint16_t xPos = GET_X_LPARAM(lParam);
		uint16_t yPos = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseMDown;
		msg.m_low.m_pos[0] = xPos;
		msg.m_low.m_pos[1] = yPos;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = xPos;
		mouseState.m_mouseY = yPos;
		mouseState.m_mouseMiddleButton = true;

		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_MBUTTONUP:
	{
		uint16_t xPos = GET_X_LPARAM(lParam);
		uint16_t yPos = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseMUp;
		msg.m_low.m_pos[0] = xPos;
		msg.m_low.m_pos[1] = yPos;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = xPos;
		mouseState.m_mouseY = yPos;
		mouseState.m_mouseMiddleButton = false;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
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

		Widgets::WidgetMgr::Get().HandleMsg(msg);


		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
		mouseState.m_mouseLeftButton = true;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
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

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
		mouseState.m_mouseLeftButton = false;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_LBUTTONDBLCLK:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseLDoubleClick;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
		mouseState.m_mouseLeftButton = true;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_RBUTTONDOWN:
	{
		SetCapture(hWnd);

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseRDown;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);


		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
		mouseState.m_mouseRightButton = true;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_RBUTTONUP:
	{
		ReleaseCapture();

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Message msg;
		msg.m_id = M_MouseRUp;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
		mouseState.m_mouseRightButton = false;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;
	
	case WM_SETCURSOR:
	{
		LPCSTR cursorResource;
		uint16_t hitResult = LOWORD(lParam);
		switch (hitResult)
		{
		case HTTOP:
		case HTBOTTOM:
			cursorResource = IDC_SIZENS;
			break;

		case HTLEFT:
		case HTRIGHT:
			cursorResource = IDC_SIZEWE;
			break;

		case HTTOPLEFT:
		case HTBOTTOMRIGHT:
			cursorResource = IDC_SIZENWSE;
			break;

		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
			cursorResource = IDC_SIZENESW;
			break;

		default:
			cursorResource = g_pIconName;
			break;
		}
		
		HCURSOR cursor = LoadCursor(NULL, cursorResource);
		SetCursor(cursor);
		return 1;
	}
		break;

	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void Update()
{
	static std::chrono::high_resolution_clock clock;
	static auto start = clock.now();

	std::chrono::nanoseconds dt = clock.now() - start;
	start = clock.now();
	std::chrono::milliseconds dtMs = std::chrono::duration_cast<std::chrono::milliseconds>(dt);

	Editors::MeshEditor::Get().Update();
	Editors::ShaderEditor::Get().Update();
	GameMgr::Get().Update();
	Widgets::WidgetMgr::Get().Update(dtMs.count());
}

void Render()
{
	Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
	Widgets::WidgetMgr& widgetsModule = Widgets::WidgetMgr::Get();

	renderModule.PreRender();

	//first render the game
	renderModule.m_gameRenderTarget->BeginScene();

	GameMgr::Get().Render();

	renderModule.m_gameRenderTarget->EndScene();

	//render the level editor
	Editors::MeshEditor::Get().Render();
	Editors::ShaderEditor::Get().Render();

	widgetsModule.Render(); //render all the viewports

	//So far everything was rendered onto target texture, now render to the frame buffer
	renderModule.BeginMainScene();
	
	// Render the widgets
	widgetsModule.Draw();

	renderModule.RenderAllText();
	renderModule.EndMainScene();
	
	renderModule.PostRender();

	//run all the render command
	renderModule.ExecuteRenderCommand();
}

bool LoadContent(const Configuration& /*config*/)
{
	//const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

	//Rendering::MeshMgr& meshMgr = Rendering::MeshMgr::Get();
	//Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

	////create the base material
	//Rendering::MaterialId baseMaterialId;
	//{
	//	Rendering::Material* pMaterial = nullptr;
	//	Rendering::MaterialMgr::Get().CreateMaterial(&pMaterial, baseMaterialId);
	//	const Systems::Asset* pAssetMaterial = assetMgr.GetAsset(Systems::AssetId(6));//vertex_color
	//	Systems::Loader::Get().LoadMaterial(pAssetMaterial->GetPath(), *pMaterial);
	//}

	////create the texture material
	//Rendering::MaterialId textureMaterialId;
	//{
	//	Rendering::Material* pMaterial = nullptr;
	//	Rendering::MaterialMgr::Get().CreateMaterial(&pMaterial, textureMaterialId);
	//	const Systems::Asset* pAssetMaterial = assetMgr.GetAsset(Systems::AssetId(4));//grid_blue
	//	Systems::Loader::Get().LoadMaterial(pAssetMaterial->GetPath(), *pMaterial);
	//}

	////load torus mesh
	//Rendering::MeshId torusMeshId;
	//{
	//	Rendering::Mesh* pCubeMesh = nullptr;
	//	meshMgr.CreateMesh(&pCubeMesh, torusMeshId);
	//	const Systems::Asset* pAsset = assetMgr.GetAsset(Systems::AssetId(3)); //basic_cube
	//	Systems::Loader::Get().LoadMesh(pAsset->GetPath(), *pCubeMesh);
	//}

	////load plane
	//Rendering::MeshId planeMeshId;
	//{
	//	Rendering::Mesh* pPlaneMesh = nullptr;
	//	meshMgr.CreateMesh(&pPlaneMesh, planeMeshId);
	//	const Systems::Asset* pAsset = assetMgr.GetAsset(Systems::AssetId(2)); //basic_plane
	//	Systems::Loader::Get().LoadMesh(pAsset->GetPath(), *pPlaneMesh);
	//}

	////basic shape material
	//{
	//	Rendering::ShaderMgr& shaderMgr = Rendering::ShaderMgr::Get();
	//	Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

	//	const std::string shaderPath = config.m_binPath + "\\";

	//	Rendering::RootSignatureId rsId = Rendering::RootSignatureMgr::Get().CreateRootSignature(shaderPath + "base-shape.rs.cso");
	//	Rendering::ShaderId vsId = shaderMgr.CreateShader(shaderPath + "base-shape.vs.cso");
	//	Rendering::ShaderId psId = shaderMgr.CreateShader(shaderPath + "base-shape.ps.cso");

	//	Rendering::PipelineStateId pid;
	//	Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().CreatePipelineState(pid);
	//	pPipelineState->Init_Generic(rsId, vsId, psId);

	//	Rendering::Material* pMaterial = nullptr;
	//	Rendering::MaterialId materialId;
	//	materialMgr.CreateMaterial(&pMaterial, materialId);
	//	pMaterial->Init(rsId, pid);

	//	renderingMgr.m_pBaseShapeMaterial = pMaterial;
	//}

	////basic shape quad
	//{
	//	Rendering::Mesh* pBasicQuadMesh = nullptr;
	//	Rendering::MeshId basicQuadMeshId;
	//	meshMgr.CreateMesh(&pBasicQuadMesh, basicQuadMeshId);

	//	const int VERTEX_COUNT = 4;
	//	Rendering::VertexPosColor vertices[VERTEX_COUNT];
	//	vertices[0].Position = DirectX::XMFLOAT3(-0.5, 0.5, 0);
	//	vertices[1].Position = DirectX::XMFLOAT3(0.5, 0.5, 0);
	//	vertices[2].Position = DirectX::XMFLOAT3(0.5, -0.5, 0);
	//	vertices[3].Position = DirectX::XMFLOAT3(-0.5, -0.5, 0);

	//	vertices[0].Color = DirectX::XMFLOAT3(1, 0, 0);
	//	vertices[1].Color = DirectX::XMFLOAT3(1, 0, 0);
	//	vertices[2].Color = DirectX::XMFLOAT3(1, 0, 0);
	//	vertices[3].Color = DirectX::XMFLOAT3(1, 0, 0);

	//	const int INDEX_COUNT = 6;
	//	uint16_t indices[INDEX_COUNT];
	//	indices[0] = 0;
	//	indices[1] = 1;
	//	indices[2] = 2;
	//	indices[3] = 2;
	//	indices[4] = 3;
	//	indices[5] = 0;
	//	pBasicQuadMesh->LoadVertexAndIndexBuffer(vertices, VERTEX_COUNT, indices, INDEX_COUNT);
	//	Rendering::RenderModule::Get().m_pBaseQuadMesh = pBasicQuadMesh;
	//}

	////Load the entities
	//GameMgr& gameMgr = GameMgr::Get();
	//gameMgr.CreatePlayerEntity(torusMeshId, baseMaterialId);

	//const DirectX::XMUINT2 gameResolution = Rendering::RenderModule::Get().GetGameResolution();
	//float aspectRatio = gameResolution.x / static_cast<float>(gameResolution.y);
	//gameMgr.CreateCameraEntity(aspectRatio);

	//gameMgr.CreateBackgroundEntity(planeMeshId, textureMaterialId);

	return true;
}

void CreateMainWindow(const Configuration& configuration)
{
	Widgets::Container* pContainer = new Widgets::Container();
	pContainer->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
	Widgets::WidgetMgr::Get().SetRoot(pContainer);

	Widgets::TabContainer* pMiddleTabContainer = new Widgets::TabContainer();
	pContainer->AddWidget(pMiddleTabContainer);

	Editors::GamePlayer::Get().CreateEditor(pMiddleTabContainer);
	Editors::LevelEditorModule::Get().CreateEditor(pMiddleTabContainer);

	Editors::MeshEditorParameter meshEditorParameter;
	meshEditorParameter.pParent = pMiddleTabContainer;
	meshEditorParameter.m_dataMaterialPath = configuration.m_dataRoot + "\\" + Systems::GetAssetFolder(Systems::kMaterial);
	meshEditorParameter.m_dataMeshPath = configuration.m_dataRoot + "\\" + Systems::GetAssetFolder(Systems::kMesh);
	meshEditorParameter.m_rawBlenderPath = configuration.m_rawBlenderPath;
	meshEditorParameter.m_editorScriptsPath = configuration.m_editorsScriptsPath;
	meshEditorParameter.m_blender = configuration.m_blender;
	Editors::MeshEditor::Get().CreateEditor(meshEditorParameter);

	Editors::ShaderEditorParameter shaderEditorParameter;
	shaderEditorParameter.m_pParent = pMiddleTabContainer;
	shaderEditorParameter.m_dataShaderPath = configuration.m_dataRoot + "\\" + Systems::GetAssetFolder(Systems::kShader);
	shaderEditorParameter.m_rawShaderPath = configuration.m_rawShadersPath;
	shaderEditorParameter.m_shaderCompilerPath = configuration.m_shaderCompiler;
	Editors::ShaderEditor::Get().CreateEditor(shaderEditorParameter);

#ifdef _DEBUG
	{
		Widgets::Tab* pTab = new Widgets::Tab();
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pLayout->GetDefaultStyle().ShowBorder(true);
		pLayout->GetDefaultStyle().SetBorderSize(1);
		pLayout->GetHoverStyle().ShowBorder(true);
		pLayout->GetHoverStyle().SetBorderSize(1);

		pTab->AddWidget(pLayout);
		pMiddleTabContainer->AddTab("Widgets", pTab);

		Widgets::Container* pOffsetContainer = new Widgets::Container(10, 20);
		pLayout->AddWidget(pOffsetContainer);

		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical);
		pVLayout->SetSizeStyle(Widgets::Layout::HSIZE_STRETCH | Widgets::Layout::VSIZE_FIT);
		pLayout->AddWidget(pVLayout);

		Widgets::Container* pTopOffsetContainer = new Widgets::Container(10, 10);
		pVLayout->AddWidget(pTopOffsetContainer);

		Widgets::TableView* pTableView = new Widgets::TableView();
		pTableView->SetSize(DirectX::XMUINT2(300, 700));

		Editors::AssetListModel* pModel = new Editors::AssetListModel(Systems::kMesh);
		pTableView->SetModel(pModel);
		pTableView->SetMultiSelection(true);
		
		pVLayout->AddWidget(pTableView);
	}
#endif

	pMiddleTabContainer->SetSelectedTab(0);
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // DEBUG

	//CommandLine cmd;
	//cmd.AddArgument("-dataroot", "", CommandLine::ARG_TYPE::STRING, CommandLine::ARG_ACTION::STORE, &g_dataRoot, "Path of the data folder.");
	//cmd.Parse(lpCmdLine);

	const int size = 256;
	char buffer[size];
	GetModuleFileNameA(NULL, buffer, size);
	std::string binPath = buffer;
	size_t lastSlash = binPath.find_last_of('\\');
	binPath = binPath.substr(0, lastSlash);

	Configuration configuration;
	std::string configurationFilename = binPath + "\\config.ini";
	configuration.Load(binPath, configurationFilename);

	Systems::ReflectionMgr::InitSingleton();
	Systems::RegisterStandardTypesToReflection();
	Systems::RegisterCoreTypesToReflection();
	Systems::RegisterSystemsTypesToReflection();

	AppResources::ResourcesMgr& resourcesMgr = AppResources::ResourcesMgr::InitSingleton();
	resourcesMgr.Init();

	Systems::AssetMgr& assetMgr = Systems::AssetMgr::InitSingleton();
	assetMgr.Init(configuration.m_dataRoot);

	Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::InitSingleton();
	std::string containerRoot = configuration.m_dataRoot;
	if (containerRoot.back() != '\\')
		containerRoot += "\\";

	containerRoot += "containers\\";

	containerMgr.Init(containerRoot);

	Systems::Loader& loader = Systems::Loader::InitSingleton();
	Systems::LoaderParameter loaderParameter;
	loaderParameter.m_dataMaterialPath = configuration.m_dataRoot + "\\" + Systems::GetAssetFolder(Systems::kMaterial);
	loaderParameter.m_dataMeshPath = configuration.m_dataRoot + "\\" + Systems::GetAssetFolder(Systems::kMesh);
	loaderParameter.m_dataShaderPath = configuration.m_dataRoot + "\\" + Systems::GetAssetFolder(Systems::kShader);
	loaderParameter.m_dataTexturePath = configuration.m_dataRoot + "\\" + Systems::GetAssetFolder(Systems::kTexture);
	loader.Init(loaderParameter);

	DirectX::XMUINT2 windowResolution(1080, 789);
	DirectX::XMUINT2 gameResolution(configuration.m_gameResolutionWidth, configuration.m_gameResolutionHeight);
	
	const char* pWindowClassName = "DX12WindowClass";
	SysWindow::RegisterWindowClass(hInstance, pWindowClassName, WndProc, resourcesMgr.GetSystemResourceId(AppResources::kAppIcon));
	g_pWindow = new SysWindow();
	g_pWindow->Create(pWindowClassName, "Alpha", windowResolution.x, windowResolution.y, hInstance);

	Rendering::RenderModule& render = Rendering::RenderModule::InitSingleton();
	render.Init(g_pWindow->GetWindowHandle(), gameResolution, windowResolution, configuration.m_binPath);

	Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::InitSingleton();
	Widgets::WidgetMgrParameter widgetMgrParameter;
	widgetMgrParameter.m_gameShaderPath = binPath;
	widgetMgrParameter.m_pMainWindow = g_pWindow;
	widgetMgr.Init(widgetMgrParameter);

	GameMgr& gameMgr = GameMgr::InitSingleton();
	gameMgr.Init();

	Inputs::InputMgr& inputMgr = Inputs::InputMgr::InitSingleton();
	inputMgr.Init();

	Editors::GamePlayer::InitSingleton();
	Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::InitSingleton();
	levelEditorModule.Init();

	Editors::MeshEditor::InitSingleton();
	Editors::ShaderEditor::InitSingleton();

	LoadContent(configuration);

	CreateMainWindow(configuration);

	g_pWindow->ShowMaximized();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		Update();
		Render();
		Inputs::InputMgr::Get().ClearAllStates();
	}

	Editors::ShaderEditor::ReleaseSingleton();
	Editors::MeshEditor::ReleaseSingleton();

	levelEditorModule.Shutdown();
	
	Editors::GamePlayer::ReleaseSingleton();

	gameMgr.Release();
	GameMgr::ReleaseSingleton();

	inputMgr.Release();
	Inputs::InputMgr::ReleaseSingleton();

	Widgets::WidgetMgr::Get().Release();
	Widgets::WidgetMgr::ReleaseSingleton();

	delete g_pWindow;

	Systems::Loader::ReleaseSingleton();
	containerMgr.Shutdown();
	Systems::ContainerMgr::ReleaseSingleton();
	Systems::AssetMgr::ReleaseSingleton();

	render.Release();
	resourcesMgr.Shutdown();

	Rendering::RenderModule::ReleaseSingleton();
	Editors::LevelEditorModule::ReleaseSingleton();
	AppResources::ResourcesMgr::ReleaseSingleton();

	Systems::ReflectionMgr::ReleaseSingleton();

	return 0;
}
