/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define NOMINMAX

#include <assert.h>
#include <cstdint>
#include <exception>
#include <chrono>
#include <windowsx.h>

#include <DirectXMath.h>

#include "Core/CommandLine.h"
#include "Core/Helper.h"

#include "Editors/GamePlayer/GamePlayer.h"
#include "Editors/LevelEditor/LevelEditor.h"
#include "Editors/MeshEditor/MeshEditor.h"
#include "Editors/ShaderEditor/ShaderEditor.h"

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

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"
#include "Systems/Loader.h"

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
			switch (wParam) //wParam is a virtual key code
			{
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;

			default:
			{
				Message msg;
				msg.m_id = M_VirtualKeyDown;
				msg.m_high = wParam; 
				Widgets::WidgetMgr::Get().HandleMsg(msg);
				Inputs::InputMgr::Get().UpdateKeyboard(wParam);
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
			DirectX::XMUINT2 size(uwidth, uheight);
			g_pWindow->Resize(uwidth, uheight);
			RenderModule::Get().ChangeMainResolution(size);
			Widgets::WidgetMgr::Get().Resize();
		}

		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	}
		break;

	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Inputs::InputMgr::MouseState mouseState;

		Message msg;
		msg.m_id = M_MouseMove;
		msg.m_low.m_pos[0] = x;
		msg.m_low.m_pos[1] = y;

		switch (wParam)
		{
			case MK_LBUTTON:
				msg.m_high = M_LButton;
				mouseState.m_mouseLeftButton = true;
				break;

			default:
				msg.m_high = M_None;
				break;
		}
		
		Widgets::WidgetMgr::Get().HandleMsg(msg);

		
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
		break;

	case WM_MOUSEWHEEL:
	{
		int16_t wheelDistance = GET_WHEEL_DELTA_WPARAM(wParam);
		uint16_t keyState = GET_KEYSTATE_WPARAM(wParam);
		uint16_t xPos = GET_X_LPARAM(lParam);
		uint16_t yPos = GET_Y_LPARAM(lParam);

		Inputs::InputMgr::MouseState mouseState;
		mouseState.m_mouseWheel = wheelDistance;
		mouseState.m_mouseX = xPos;
		mouseState.m_mouseY = yPos;
		if (keyState & MK_LBUTTON)
			mouseState.m_mouseLeftButton = true;
		if (keyState & MK_MBUTTON)
			mouseState.m_mouseMiddleButton = true;
		if (keyState & MK_RBUTTON)
			mouseState.m_mouseRightButton = true;

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


		Inputs::InputMgr::MouseState mouseState;
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

		Inputs::InputMgr::MouseState mouseState;
		mouseState.m_mouseX = x;
		mouseState.m_mouseY = y;
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

	Editors::MeshEditor::Get().Update();
	Editors::ShaderEditor::Get().Update();
	Editors::LevelEditor::Get().Update();
	GameMgr::Get().Update();
	Widgets::WidgetMgr::Get().Update();
}

void Render()
{

	RenderModule& renderModule = RenderModule::Get();

	renderModule.PreRender();

	//first render the game
	renderModule.m_gameRenderTarget->BeginScene();

	GameMgr::Get().Render();

	renderModule.m_gameRenderTarget->EndScene();

	//render the level editor
	Editors::LevelEditor::Get().Render();
	Editors::MeshEditor::Get().Render();
	Editors::ShaderEditor::Get().Render();

	//So far everything was rendered onto target texture, now render to the frame buffer
	renderModule.BeginMainScene();
	
	// Render the widgets
	Widgets::WidgetMgr::Get().Draw();

	renderModule.RenderAllText();
	renderModule.EndMainScene();
	
	renderModule.PostRender();

	//run all the render command
	renderModule.ExecuteRenderCommand();
}

bool LoadContent()
{
	const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

	Rendering::MeshMgr& meshMgr = Rendering::MeshMgr::Get();

	//create the base material
	Rendering::MaterialId baseMaterialId;
	{
		Rendering::Material* pMaterial = nullptr;
		Rendering::MaterialMgr::Get().CreateMaterial(&pMaterial, baseMaterialId);
		const Systems::Asset* pAssetMaterial = assetMgr.GetAsset(Systems::AssetId(6));//vertex_color
		Systems::Loader::Get().LoadMaterial(pAssetMaterial->GetPath(), *pMaterial);
	}

	//create the texture material
	Rendering::MaterialId textureMaterialId;
	{
		Rendering::Material* pMaterial = nullptr;
		Rendering::MaterialMgr::Get().CreateMaterial(&pMaterial, textureMaterialId);
		const Systems::Asset* pAssetMaterial = assetMgr.GetAsset(Systems::AssetId(4));//grid_blue
		Systems::Loader::Get().LoadMaterial(pAssetMaterial->GetPath(), *pMaterial);
	}

	//load torus mesh
	Rendering::MeshId torusMeshId;
	{
		Rendering::Mesh* pCubeMesh = nullptr;
		meshMgr.CreateMesh(&pCubeMesh, torusMeshId);
		const Systems::Asset* pAsset = assetMgr.GetAsset(Systems::AssetId(3)); //basic_cube
		Systems::Loader::Get().LoadMesh(pAsset->GetPath(), *pCubeMesh);
	}

	//load plane
	Rendering::MeshId planeMeshId;
	{
		Rendering::Mesh* pPlaneMesh = nullptr;
		meshMgr.CreateMesh(&pPlaneMesh, planeMeshId);
		const Systems::Asset* pAsset = assetMgr.GetAsset(Systems::AssetId(2)); //basic_plane
		Systems::Loader::Get().LoadMesh(pAsset->GetPath(), *pPlaneMesh);
	}

	//Load the entities
	GameMgr& gameMgr = GameMgr::Get();
	gameMgr.CreatePlayerEntity(torusMeshId, baseMaterialId);

	const DirectX::XMUINT2 gameResolution = RenderModule::Get().GetGameResolution();
	float aspectRatio = gameResolution.x / static_cast<float>(gameResolution.y);
	gameMgr.CreateCameraEntity(aspectRatio);

	gameMgr.CreateBackgroundEntity(planeMeshId, textureMaterialId);

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
	Editors::LevelEditor::Get().CreateEditor(pMiddleTabContainer);

	Editors::MeshEditorParameter meshEditorParameter;
	meshEditorParameter.pParent = pMiddleTabContainer;
	meshEditorParameter.m_dataMaterialPath = configuration.m_dataMaterialsPath;
	meshEditorParameter.m_dataMeshPath = configuration.m_dataMeshPath;
	meshEditorParameter.m_editorIconsPath = configuration.m_editorsIconsPath;
	meshEditorParameter.m_rawBlenderPath = configuration.m_rawBlenderPath;
	meshEditorParameter.m_editorScriptsPath = configuration.m_editorsScriptsPath;
	meshEditorParameter.m_blender = configuration.m_blender;
	Editors::MeshEditor::Get().CreateEditor(meshEditorParameter);

	Editors::ShaderEditorParameter shaderEditorParameter;
	shaderEditorParameter.m_pParent = pMiddleTabContainer;
	shaderEditorParameter.m_dataShaderPath = configuration.m_dataShadersPath;
	shaderEditorParameter.m_rawShaderPath = configuration.m_rawShadersPath;
	shaderEditorParameter.m_shaderCompilerPath = configuration.m_shaderCompiler;
	Editors::ShaderEditor::Get().CreateEditor(shaderEditorParameter);

	{
		Widgets::Tab* pTab = new Widgets::Tab();
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pLayout->GetDefaultStyle().ShowBorder(true);
		pLayout->GetDefaultStyle().SetBorderSize(1);

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

		Widgets::TextBox* pNewTextBox = new Widgets::TextBox();
		pNewTextBox->SetSize(DirectX::XMUINT2(100, 20));
		pNewTextBox->SetText("This is a dummy text");

		pVLayout->AddWidget(pNewTextBox);
	}

	pMiddleTabContainer->SetSelectedTab(0);
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
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
	configuration.Load(configurationFilename);

	Systems::Loader& loader = Systems::Loader::InitSingleton();
	Systems::LoaderParameter loaderParameter;
	loaderParameter.m_dataMaterialPath = configuration.m_dataMaterialsPath;
	loaderParameter.m_dataMeshPath = configuration.m_dataMeshPath;
	loaderParameter.m_dataShaderPath = configuration.m_dataShadersPath;
	loaderParameter.m_dataTexturePath = configuration.m_dataTexturesPath;
	loader.Init(loaderParameter);

	Systems::AssetMgr& assetMgr = Systems::AssetMgr::InitSingleton();
	assetMgr.Init(configuration.m_dataRoot);
	assetMgr.LoadAllAssets();

	DirectX::XMUINT2 windowResolution(1080, 789);
	DirectX::XMUINT2 gameResolution(configuration.m_gameResolutionWidth, configuration.m_gameResolutionHeight);
	
	std::string iconPath = configuration.m_editorsIconsPath + "\\alpha_white.ico";
	const char* pWindowClassName = "DX12WindowClass";
	SysWindow::RegisterWindowClass(hInstance, pWindowClassName, WndProc, iconPath);
	g_pWindow = new SysWindow();
	g_pWindow->Create(pWindowClassName, "Alpha", windowResolution.x, windowResolution.y, hInstance);

	RenderModule& render = RenderModule::InitSingleton();
	render.Init(g_pWindow->GetWindowHandle(), gameResolution, windowResolution);

	Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::InitSingleton();
	Widgets::WidgetMgrParameter widgetMgrParameter;
	widgetMgrParameter.m_editorFontsPath = configuration.m_editorsFontsPath;
	widgetMgrParameter.m_editorIconsPath = configuration.m_editorsIconsPath;
	widgetMgrParameter.m_gameShaderPath = binPath;
	widgetMgrParameter.m_pMainWindow = g_pWindow;
	widgetMgr.Init(widgetMgrParameter);

	GameMgr& gameMgr = GameMgr::InitSingleton();
	gameMgr.Init();

	Inputs::InputMgr& inputMgr = Inputs::InputMgr::InitSingleton();
	inputMgr.Init();

	Editors::GamePlayer::InitSingleton();
	Editors::LevelEditor::InitSingleton();
	Editors::MeshEditor::InitSingleton();
	Editors::ShaderEditor::InitSingleton();

	LoadContent();

	CreateMainWindow(configuration);

	g_pWindow->Show();

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
	Editors::LevelEditor::ReleaseSingleton();
	Editors::GamePlayer::ReleaseSingleton();

	gameMgr.Release();
	GameMgr::ReleaseSingleton();

	inputMgr.Release();
	Inputs::InputMgr::ReleaseSingleton();

	Widgets::WidgetMgr::Get().Release();
	Widgets::WidgetMgr::ReleaseSingleton();

	delete g_pWindow;

	Systems::Loader::ReleaseSingleton();
	Systems::AssetMgr::ReleaseSingleton();

	render.Release();
	RenderModule::ReleaseSingleton();

	return 0;
}
