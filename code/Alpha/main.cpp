/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#define NOMINMAX

#include <assert.h>
#include <cmath>
#include <cstdint>
#include <exception>
#include <windowsx.h>

#include <DirectXMath.h>

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Inputs/GameCommands.h"
#include "Alpha/Configuration.h"
#include "Alpha/Reflection/ReflectionGameTypes.h"

#include "Core/CommandLine.h"
#include "Core/Helper.h"
#include "Core/Log/LogModule.h"
#include "Core/Reflection/ReflectionMgr.h"

#include "Editors/EditorManager.h"
#include "Editors/EditorParameter.h"
#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LogEditor/LogEditor.h"
#include "Editors/MaterialEditor/MaterialEditorModule.h"
#include "Editors/MeshEditor/MeshEditorModule.h"
#include "Editors/ObjectWatcher/ObjectWatcher.h"
#include "Editors/ParticleEditor/ParticleEditorModule.h"
#include "Editors/TextureEditor/TextureEditorModule.h"
#include "Editors/MeshEditor/MeshListModel.h"
#include "Inputs/InputMgr.h"

#include "OsWin/Cursor/Cursor.h"
#include "OsWin/UIMessage/UIMessage.h"
#include "OsWin/UIMessage/UIMessageId.h"

#include "Rendering/Reflection/ReflectionRenderingTypes.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Resources/ResourcesMgr.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Assets/AssetObjects/Texture/CubemapAsset.h"
#include "Systems/Assets/AssetObjects/Texture/Texture2DAsset.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"
#include "Systems/Clock/Clock.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Reflection/ReflectionCoreTypes.h"
#include "Systems/Reflection/ReflectionStandardTypes.h"
#include "Systems/Reflection/ReflectionSystemsTypes.h"

#include "OsWin/SysWindow.h"

#include "Widgets/Button.h"
#include "Widgets/Container.h"
#include "Widgets/Layout.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Split.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/TextBox.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/CheckBox.h"
#include "Widgets/Widgets/ComboBox.h"
#include "Widgets/Widgets/Container_V2.h"
#include "Widgets/Widgets/TableView.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // DEBUG

//#pragma optimize("", off)

Os::SysWindow* g_pWindow = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCT* pCreateStruct = reinterpret_cast<const CREATESTRUCT*>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;

	case WM_CHAR:
	{
		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::CharKeyDown;
		msg.m_high.m_uint64 = wParam;
		Widgets::WidgetMgr::Get().HandleMsg(msg);
	}
	break;

	case WM_KEYDOWN:
	{
		//wParam is a virtual key code
		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::VirtualKeyDown;
		msg.m_high.m_uint64 = wParam;
		Widgets::WidgetMgr::Get().HandleMsg(msg);
		Inputs::InputMgr::Get().UpdateKeyboardState(wParam, true);
	}
	break;

	case WM_KEYUP:
	{
		//wParam is a virtual key code
		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::VirtualKeyUp;
		msg.m_high.m_uint64 = wParam;
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
		int32_t x = GET_X_LPARAM(lParam);
		int32_t y = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseMove;
		msg.m_low.m_uint32[0] = x;
		msg.m_low.m_uint32[1] = y;
		msg.m_high.m_uint64 = wParam;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(x, y);
		mouseState.m_mouseLeftButton = wParam & MK_LBUTTON;
		mouseState.m_mouseRightButton = wParam & MK_RBUTTON;
		mouseState.m_mouseMiddleButton = wParam & MK_MBUTTON;
		inputMgr.UpdateMouseState(mouseState);
	}
		break;

	case WM_MOUSEWHEEL:
	{
		int16_t wheelDistance = GET_WHEEL_DELTA_WPARAM(wParam);
		POINT mousePoint;
		mousePoint.x = GET_X_LPARAM(lParam);
		mousePoint.y = GET_Y_LPARAM(lParam);

		//WM_MOUSEWHEEL returns the mouse position in the screen space. I need to convert it to client space.
		ScreenToClient(hWnd, &mousePoint);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseWheel;
		msg.m_high.m_int64 = wheelDistance;
		msg.m_low.m_uint32[0] = mousePoint.x;
		msg.m_low.m_uint32[1] = mousePoint.y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();

		mouseState.m_mouseWheel = wheelDistance;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);

	}
		break;

	case WM_MBUTTONDOWN:
	{
		int32_t xPos = GET_X_LPARAM(lParam);
		int32_t yPos = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseMDown;
		msg.m_low.m_uint32[0] = xPos;
		msg.m_low.m_uint32[1] = yPos;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(xPos, yPos);
		mouseState.m_mouseMiddleButton = true;

		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_MBUTTONUP:
	{
		int32_t xPos = GET_X_LPARAM(lParam);
		int32_t yPos = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseMUp;
		msg.m_low.m_uint32[0] = xPos;
		msg.m_low.m_uint32[1] = yPos;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(xPos, yPos);
		mouseState.m_mouseMiddleButton = false;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_LBUTTONDOWN:
	{
		SetCapture(hWnd);

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseLDown;
		msg.m_low.m_uint32[0] = x;
		msg.m_low.m_uint32[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);


		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(x, y);
		mouseState.m_mouseLeftButton = true;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_LBUTTONUP:
	{
		ReleaseCapture();

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseLUp;
		msg.m_low.m_uint32[0] = x;
		msg.m_low.m_uint32[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(x, y);
		mouseState.m_mouseLeftButton = false;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_LBUTTONDBLCLK:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseLDoubleClick;
		msg.m_low.m_uint32[0] = x;
		msg.m_low.m_uint32[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(x, y);
		mouseState.m_mouseLeftButton = true;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_RBUTTONDOWN:
	{
		SetCapture(hWnd);

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseRDown;
		msg.m_low.m_uint32[0] = x;
		msg.m_low.m_uint32[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);


		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(x, y);
		mouseState.m_mouseRightButton = true;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;

	case WM_RBUTTONUP:
	{
		ReleaseCapture();

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Os::UIMessage msg;
		msg.m_id = Os::UIMessageId::MouseRUp;
		msg.m_low.m_uint32[0] = x;
		msg.m_low.m_uint32[1] = y;

		Widgets::WidgetMgr::Get().HandleMsg(msg);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		Inputs::InputMgr::MouseState mouseState = inputMgr.GetMouseState();
		mouseState.m_mousePos = Core::Int2(x, y);
		mouseState.m_mouseRightButton = false;
		Inputs::InputMgr::Get().UpdateMouseState(mouseState);
	}
	break;
	
	case WM_SETCURSOR:
	{
		Os::CursorId cursor;
		uint16_t hitResult = LOWORD(lParam);
		switch (hitResult)
		{
		case HTTOP:
		case HTBOTTOM:
			cursor = Os::CursorId::ResizeVertical;
			break;

		case HTLEFT:
		case HTRIGHT:
			cursor = Os::CursorId::ResizeHorizontal;
			break;

		case HTTOPLEFT:
		case HTBOTTOMRIGHT:
			cursor = Os::CursorId::ResizeDiagonalNWSE;
			break;

		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
			cursor = Os::CursorId::ResizeDiagonalNESW;
			break;

		default:
			Widgets::WidgetMgr::Get().ResetCursorId();
			return 1;
			break;
		}
		
		Os::SetCursor(cursor);
		return 1;
	}
	break;

	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void Update(float dt)
{
	Inputs::InputMgr::Get().PreUpdate();

	//start with a garbage collect
	Systems::ContainerMgr::Get().GarbageCollect();

	Systems::GameMgr::Get().Update(dt);

	//update widgets
	uint64_t dtMs = static_cast<uint64_t>(dt * 1000);
	Widgets::WidgetMgr::Get().Update(dtMs);

	Inputs::InputMgr::Get().PostUpdate();
}

void Render()
{
	Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
	Widgets::WidgetMgr& widgetsModule = Widgets::WidgetMgr::Get();

	renderModule.PreRender();

	//first render the game
	Systems::GameMgr::Get().Render();

	//render all the viewports from the editors
	widgetsModule.Render();

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

void CreateMainWindow(const std::string& shaderPath)
{
	Widgets::Container* pContainer = new Widgets::Container();
	pContainer->SetSizeStyle(Widgets::HSIZE_STRETCH | Widgets::VSIZE_STRETCH);
	Widgets::WidgetMgr::Get().SetRoot(pContainer);

	Widgets::TabContainer* pMiddleTabContainer = new Widgets::TabContainer();
	pContainer->AddWidget(pMiddleTabContainer);

	//Editors::GamePlayer::Get().CreateEditor(pMiddleTabContainer);

	Editors::EditorParameter parameter;
	parameter.m_pParent = pMiddleTabContainer;
	parameter.m_shaderPath = shaderPath;
	Editors::EditorManager::Get().Init(parameter);

	Editors::LogEditorParameter logEditorParameter;
	logEditorParameter.m_pParent = pMiddleTabContainer;
	Editors::LogEditor::Get().CreateEditor(logEditorParameter);

#ifdef _DEBUG
	{
		Widgets::Tab* pTab = new Widgets::Tab();
		//Widgets::Layout* pLayout = new Widgets::Layout();
		//pLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		/*pLayout->GetDefaultStyle().ShowBorder(true);
		pLayout->GetDefaultStyle().SetBorderSize(1);
		pLayout->GetHoverStyle().ShowBorder(true);
		pLayout->GetHoverStyle().SetBorderSize(3);*/

		//pTab->AddWidget(pLayout);
		pMiddleTabContainer->AddTab("Widgets", pTab);

		//Widgets::Container_V2* pOffsetContainer = new Widgets::Container_V2();
		//Widgets::Container* pOffsetContainer = new Widgets::Container();
		//pOffsetContainer->SetSizeStyle(Widgets::Widget::DEFAULT);
		//pOffsetContainer->SetSize(Core::UInt2(500, 200));
		//pOffsetContainer->SetX(20);
		//pOffsetContainer->SetY(30);
		//pTab->AddWidget(pOffsetContainer);
		//pOffsetContainer->GetDefaultStyle().ShowBorder(true);
		//pOffsetContainer->GetDefaultStyle().SetBorderSize(2);
		//pLayout->AddWidget(pOffsetContainer);

		Widgets::TableView* pTableView = new Widgets::TableView();
		pTableView->SetSizeStyle(Widgets::DEFAULT);
		pTableView->SetSize(Core::UInt2(500, 200));
		pTableView->SetX(20);
		pTableView->SetY(30);
		pTab->AddWidget(pTableView);

		//pTableView->SetSize(Core::UInt2(300, 700));

		Editors::MeshListModel* pModel = new Editors::MeshListModel();
		pTableView->SetModel(pModel);
		pTableView->SetColumnWidth(Editors::MeshListModel::Columns::Id, 200);
		pTableView->SetColumnWidth(Editors::MeshListModel::Columns::Name, 300);
		//Editors::AssetListModel* pModel = new Editors::AssetListModel(Systems::kMesh);
		//pTableView->SetModel(pModel);
		//pTableView->SetMultiSelection(true);
	}
#endif

	pMiddleTabContainer->SetSelectedTab(0);
	//pMiddleTabContainer->SetSelectedTab(SID("Widgets"));
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// The output window will print the memory leak with an allocation number. Use this number here to break in the debugger during the allocation.
	//_crtBreakAlloc = 5683;
#endif // DEBUG

	//CommandLine cmd;
	//cmd.AddArgument("-dataroot", "", CommandLine::ARG_TYPE::STRING, CommandLine::ARG_ACTION::STORE, &g_dataRoot, "Path of the data folder.");
	//cmd.Parse(lpCmdLine);

	Systems::Clock& clock = Systems::Clock::InitSingleton();
	clock.Init();

	const int size = 256;
	char buffer[size];
	GetModuleFileNameA(NULL, buffer, size);
	std::string binPath = buffer;
	size_t lastSlash = binPath.find_last_of('\\');
	binPath = binPath.substr(0, lastSlash);

	Configuration configuration;
	std::string configurationFilename = binPath + "\\config.ini";
	configuration.Load(binPath, configurationFilename);

	Core::ReflectionMgr::InitSingleton();
	Systems::RegisterStandardTypesToReflection();
	Rendering::RegisterRenderingTypesToReflection();
	Systems::RegisterCoreTypesToReflection();
	Systems::RegisterSystemsTypesToReflection();
	RegisterGameTypesToReflection();

	Core::LogModule& logModule = Core::LogModule::InitSingleton();
	logModule.Init();
	logModule.LogInfo("Engine Alpha 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>");

	AppResources::ResourcesMgr& resourcesMgr = AppResources::ResourcesMgr::InitSingleton();
	resourcesMgr.Init();

	Systems::AssetMgr& assetMgr = Systems::AssetMgr::InitSingleton();
	assetMgr.Init(configuration.m_dataRoot);
	assetMgr.RegisterAssetType<Systems::MeshAsset>();
	assetMgr.RegisterAssetType<Systems::MaterialAsset>();
	assetMgr.RegisterAssetType<Systems::MaterialInstanceAsset>();
	assetMgr.RegisterAssetType<Systems::LevelAsset>();
	assetMgr.RegisterAssetType<Systems::Texture2DAsset>();
	assetMgr.RegisterAssetType<Systems::CubemapAsset>();
	assetMgr.RegisterAssetType<Systems::ParticleEffectAsset>();
	assetMgr.LoadMetadataTable();
	

	Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::InitSingleton();
	std::string containerRoot = configuration.m_dataRoot;
	if (containerRoot.back() != '\\')
		containerRoot += "\\";

	containerRoot += "containers\\";

	containerMgr.Init(containerRoot);

	DirectX::XMUINT2 windowResolution(1080, 789);
	DirectX::XMUINT2 gameResolution(configuration.m_gameResolutionWidth, configuration.m_gameResolutionHeight);
	
	const char* pWindowClassName = "DX12WindowClass";
	Os::SysWindow::RegisterWindowClass(hInstance, pWindowClassName, WndProc, resourcesMgr.GetSystemResourceId(AppResources::kAppIcon));
	g_pWindow = new Os::SysWindow();
	g_pWindow->Create(pWindowClassName, "Alpha", windowResolution.x, windowResolution.y, hInstance);

	Rendering::RenderModule& render = Rendering::RenderModule::InitSingleton();
	render.Init(g_pWindow->GetWindowHandle(), gameResolution, windowResolution, configuration.m_binPath);

	Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::InitSingleton();
	Widgets::WidgetMgrParameter widgetMgrParameter;
	widgetMgrParameter.m_gameShaderPath = binPath;
	widgetMgrParameter.m_pMainWindow = g_pWindow;
	widgetMgr.Init(widgetMgrParameter);

	Systems::GameMgr& gameMgr = Systems::GameMgr::InitSingleton();
	gameMgr.Init();

	Inputs::InputMgr& inputMgr = Inputs::InputMgr::InitSingleton();
	inputMgr.Init();
	GameCommands::RegisterGameCommands();

	Editors::ObjectWatcher::InitSingleton();
	Editors::ObjectWatcher& objectWatcher = Editors::ObjectWatcher::Get();
	objectWatcher.Init();

	Editors::ParticleEditorModule& particleEditorModule = Editors::ParticleEditorModule::InitSingleton();
	particleEditorModule.Init();

	Editors::TextureEditorModule& textureEditorModule = Editors::TextureEditorModule::InitSingleton();
	textureEditorModule.Init();

	Editors::MeshEditorModule& meshEditorModule = Editors::MeshEditorModule::InitSingleton();
	meshEditorModule.Init();

	Editors::EditorManager& editorManager = Editors::EditorManager::InitSingleton();

	Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::InitSingleton();
	levelEditorModule.Init();

	Editors::MaterialEditorModule& materialEditorModule = Editors::MaterialEditorModule::InitSingleton();
	materialEditorModule.Init(configuration.m_dataRoot);

	Editors::LogEditor& logEditor = Editors::LogEditor::InitSingleton();
	logEditor.Init();

	BulletSubsystem* pBulletSubsystem = new BulletSubsystem();
	BulletSubsystem::m_subsystemIndex = gameMgr.RegisterGameSubsystem(pBulletSubsystem);

	CreateMainWindow(binPath);

	g_pWindow->ShowMaximized();

	const int FPS = 60;
	constexpr float FRAME_DURATION = 1.f / FPS;

	float previousApplicationTick = clock.GetApplicationTime();

	uint32_t updateFPSCounter = 0;
	float updateFPSTimer = clock.GetRealApplicationTime();

	uint32_t renderFPSCounter = 0;
	float renderFPSTimer = clock.GetRealApplicationTime();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		//always peek so I don't miss any message
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	
		//now compute elapsed time
		clock.Update();

		float currentApplicationTick = clock.GetApplicationTime();
		float applicationDt = currentApplicationTick - previousApplicationTick;

		//update every frame duration
		if (applicationDt >= FRAME_DURATION)
		{
			//Core::LogModule::Get().LogInfo("dt %f", applicationDt);
			Update(applicationDt);

			++updateFPSCounter;

			if (updateFPSTimer + 1 <= clock.GetRealApplicationTime())
			{
				//Core::LogModule::Get().LogInfo("UPDATE FPS %d", updateFPSCounter);
				updateFPSCounter = 0;
				updateFPSTimer = clock.GetRealApplicationTime();
			}
		
			previousApplicationTick = currentApplicationTick;
		}

		{
			Render();
			++renderFPSCounter;
			if (renderFPSTimer + 1 <= clock.GetRealApplicationTime())
			{
				//Core::LogModule::Get().LogInfo("RENDER FPS %d", renderFPSCounter);
				renderFPSCounter = 0;
				renderFPSTimer = clock.GetRealApplicationTime();
			}
		}	
	}

	logEditor.Shutdown();
	Editors::LogEditor::ReleaseSingleton();

	editorManager.Shutdown();
	Editors::EditorManager::ReleaseSingleton();

	meshEditorModule.Shutdown();
	Editors::MeshEditorModule::ReleaseSingleton();

	textureEditorModule.Shutdown();
	Editors::TextureEditorModule::ReleaseSingleton();

	materialEditorModule.Shutdown();
	Editors::MaterialEditorModule::ReleaseSingleton();

	particleEditorModule.Shutdown();
	Editors::ParticleEditorModule::ReleaseSingleton();

	levelEditorModule.Shutdown();

	gameMgr.Release();
	Systems::GameMgr::ReleaseSingleton();

	inputMgr.Release();
	Inputs::InputMgr::ReleaseSingleton();

	Widgets::WidgetMgr::Get().Release();
	Widgets::WidgetMgr::ReleaseSingleton();

	delete g_pWindow;

	containerMgr.Shutdown();
	Systems::ContainerMgr::ReleaseSingleton();
	Systems::AssetMgr::ReleaseSingleton();

	render.Release();
	resourcesMgr.Shutdown();

	Rendering::RenderModule::ReleaseSingleton();
	
	Editors::LevelEditorModule::ReleaseSingleton();

	objectWatcher.Shutdown();
	Editors::ObjectWatcher::ReleaseSingleton();

	AppResources::ResourcesMgr::ReleaseSingleton();

	logModule.Shutdown();
	Core::LogModule::ReleaseSingleton();

	Core::ReflectionMgr::ReleaseSingleton();

	clock.Shutdown();
	Systems::Clock::Get().ReleaseSingleton();

	return 0;
}
