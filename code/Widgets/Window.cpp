/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Window.h"

#include "Rendering/Font/Font.h"
#include "Rendering/RenderModule.h"

#include "Widgets/Container.h"
#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

#include "OsWin/SysWindow.h"

extern SysWindow* g_pWindow;

namespace Widgets
{

Window::Window(DirectX::XMUINT2 size)
	: Widget(size.x, size.y, 0, 0)
	, m_previousMousePosition()
	, m_isMaximized(false)
	, m_pContainer(nullptr)
	, m_drag(false)
{
	SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);

	//Full window vlayout
	Layout* pWindowLayout = new Layout(0, 0, 0, 0);
	pWindowLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	pWindowLayout->SetDirection(Layout::Vertical);

	//Header layout
	int menuBarHeight = 30;
	Layout* pLayout = new Layout(1000, menuBarHeight, 0, 0);
	pLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
	pLayout->SetDirection(Layout::Horizontal);
	pLayout->OnClick([this](int, int) -> bool 
	{
		m_drag = true;
		m_previousMousePosition = WidgetMgr::Get().GetCursorPosition();
		return true;
		});

	pLayout->OnLeftMouseUp([this](int, int) -> bool
	{
		m_drag = false;
		return true;
	});

	pWindowLayout->AddWidget(pLayout);

	//Header left layout containig menus
	Layout* pMenuBarLayout = new Layout(400, menuBarHeight, 0, 0);
	pMenuBarLayout->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_STRETCH);
	pMenuBarLayout->SetDirection(Layout::Horizontal);
	pLayout->AddWidget(pMenuBarLayout);

	//Header right containing close/minimize/maximize buttons
	Layout* pButtonLayout = new Layout(0, menuBarHeight, 0, 0);
	pButtonLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	pButtonLayout->SetDirection(Layout::Horizontal_Reverse);
	pLayout->AddWidget(pButtonLayout);

	//alpha icon
	//int iconSize = 20;
	//int iconY = (menuBarHeight - iconSize) / 2;
	//Icon* pIcon = new Icon(DirectX::XMINT2(0, iconY), DirectX::XMUINT2(iconSize, iconSize), g_dataRoot + "\\textures\\alpha_64.png");

	int buttonHeight = 20;
	//int buttonWidth = 50;
	int buttonY = (menuBarHeight - buttonHeight) / 2;
	//int labelX = 10;

	int labelPadding = 10;
	int buttonSize = labelPadding * 2;

	//File button
	std::string fileText = "File";
	DirectX::XMUINT2 textSize;
	const Font* pFont = RenderModule::Get().GetFontMgr().GetResource(WidgetMgr::Get().GetUIFontId());
	pFont->ComputeRect(fileText, textSize);
	Button* pButton1 = new Button(textSize.x + buttonSize, buttonHeight, 0, buttonY);
	pButton1->OnClick([](int, int) -> bool {
		OutputDebugString("Click on File button\n");
		return true;
		});

	float labelScale = 1;
	Label* pLabel1 = new Label(labelPadding, 0, labelScale, fileText);
	pButton1->AddWidget(pLabel1);

	//Edit button
	fileText = "Edit";
	pFont->ComputeRect(fileText, textSize);
	Button* pButton2 = new Button(textSize.x + buttonSize, buttonHeight, 0, buttonY);
	pButton2->OnClick([](int, int) -> bool {
		OutputDebugString("Click on Edit button\n");
		return true;
		});
	Label* pLabel2 = new Label(labelPadding, 0, labelScale, fileText);
	pButton2->AddWidget(pLabel2);

	//Window button
	fileText = "Window";
	pFont->ComputeRect(fileText, textSize);
	Button* pButton3 = new Button(textSize.x + buttonSize, buttonHeight, 0, buttonY);
	pButton3->OnClick([](int, int) -> bool {
		OutputDebugString("Click on Window button\n");
		return true;
		});
	Label* pLabel3 = new Label(labelPadding, 0, labelScale, fileText);
	pButton3->AddWidget(pLabel3);

	//pMenuBarLayout->AddWidget(pIcon);
	pMenuBarLayout->AddWidget(pButton1);
	pMenuBarLayout->AddWidget(pButton2);
	pMenuBarLayout->AddWidget(pButton3);

	//Close button
	Button* pCloseButton = new Button(20, 0, 0, 0);
	pCloseButton->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_STRETCH);
	//Icon* pCloseIcon = new Icon(DirectX::XMINT2(0, 0), DirectX::XMUINT2(16, 16), g_dataRoot + "\\textures\\icon_close_16.png");
	//pCloseIcon->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
	//pCloseButton->AddWidget(pCloseIcon);
	pCloseButton->OnClick([](int, int)-> bool {
		OutputDebugString("Click on close button\n");
		SendMessage(g_pWindow->GetWindowHandle(), WM_DESTROY, 0, 0);
		return true;
		});

	//Max button
	Button* pMaxButton = new Button(20, 0, 0, 0);
	pMaxButton->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_STRETCH);
	//Icon* pMaxIcon = new Icon(DirectX::XMINT2(0, 0), DirectX::XMUINT2(16, 16), g_dataRoot + "\\textures\\icon_maximize_16.png");
	//pMaxIcon->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
	//Icon* pRestoreIcon = new Icon(DirectX::XMINT2(0, 0), DirectX::XMUINT2(16, 16), g_dataRoot + "\\textures\\icon_restore_16.png");
	//pRestoreIcon->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
	//pMaxButton->AddWidget(pMaxIcon);
	/*pMaxButton->OnClick([this, pMaxButton, pMaxIcon, pRestoreIcon](int, int)-> bool {
		OutputDebugString("Click on maximize/restore button\n");
		if (m_isMaximized)
		{
			pMaxButton->RemoveWidget(pRestoreIcon);
			pMaxButton->AddWidget(pMaxIcon);

			ShowWindow(g_pWindow->GetWindowHandle(), SW_SHOWNORMAL);
		}
		else
		{
			pMaxButton->RemoveWidget(pMaxIcon);
			pMaxButton->AddWidget(pRestoreIcon);
			ShowWindow(g_pWindow->GetWindowHandle(), SW_SHOWMAXIMIZED);
		}
		m_isMaximized = !m_isMaximized;
		return true;
		});*/

	//Min button
	Button* pMinButton = new Button(20, 0, 0, 0);
	pMinButton->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_STRETCH);
	//Icon* pMinIcon = new Icon(DirectX::XMINT2(0, 0), DirectX::XMUINT2(16, 16), g_dataRoot + "\\textures\\icon_minimize_16.png");
	//pMinIcon->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
	//pMinButton->AddWidget(pMinIcon);
	/*pMinButton->OnClick([](int, int)-> bool {
		OutputDebugString("Click on minimize button\n");
		ShowWindow(g_pWindow->GetWindowHandle(), SW_SHOWMINIMIZED);
		return true;
		});*/

	pButtonLayout->AddWidget(pCloseButton);
	pButtonLayout->AddWidget(pMaxButton);
	pButtonLayout->AddWidget(pMinButton);

	//Add the container
	m_pContainer = new Widgets::Container();
	pWindowLayout->AddWidget(m_pContainer);
	pWindowLayout->SetName("window_layout");

	Widget::AddWidget(pWindowLayout);
}

Window::~Window()
{}

void Window::Update()
{
	if (!m_drag)
		return;

	DirectX::XMINT2 m_currentMousePosition = WidgetMgr::Get().GetCursorPosition();

	int dtX = m_currentMousePosition.x - m_previousMousePosition.x;
	int dtY = m_currentMousePosition.y - m_previousMousePosition.y;

	RECT r = g_pWindow->GetWindowRectangle();
	MoveWindow(g_pWindow->GetWindowHandle(), r.left + dtX, r.top + dtY, r.right - r.left, r.bottom - r.top, true);

	m_previousMousePosition.x = m_currentMousePosition.x;
	m_previousMousePosition.y = m_currentMousePosition.y;
	Widget::Update();
}

void Window::Draw(const DirectX::XMFLOAT2& windowSize)
{
	Widget::Draw(windowSize);
}

void Window::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
{
	Widget::Resize(parentAbsPos, parentSize);
}

void Window::AddWidget(Widget* pWidget)
{
	m_pContainer->AddWidget(pWidget);
}

void Window::RemoveWidget(const Widget* pWidget)
{
	m_pContainer->RemoveWidget(pWidget);
}

}
