/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Window.h"

#include <algorithm>
#include <assert.h>
#include <cstdio>

Window::Window()
	: m_hWindow(nullptr)
	, m_fullscreen(false)
	, m_width(0)
	, m_height(0)
	, m_windowRectangle()
{}

Window::~Window()
{}

bool Window::Create(const wchar_t* pWindowClassName, const wchar_t* pWindowTitle, uint32_t width, uint32_t height, HINSTANCE hInstance)
{
	m_width = width;
	m_height = height;

	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = static_cast<LONG>(width);
	windowRect.bottom = static_cast<LONG>(height);
	
	int windowStyle = WS_OVERLAPPEDWINDOW;
	::AdjustWindowRect(&windowRect, windowStyle, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	m_hWindow = ::CreateWindowEx(0, pWindowClassName, pWindowTitle, windowStyle, windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);
	//SetWindowLong(m_hWindow, GWL_STYLE, 0);
	if (m_hWindow == 0)
	{
		DWORD error = GetLastError();
		printf("%d", error);
		assert(m_hWindow && "Failed to create window");
	}

	return true;
}

void Window::SetFullscreen(bool fullscreen)
{
	if (m_fullscreen != fullscreen)
	{
		m_fullscreen = fullscreen;

		if (m_fullscreen) // Switching to fullscreen.
		{
			// Store the current window dimensions so they can be restored 
			// when switching out of fullscreen state.
			::GetWindowRect(m_hWindow, &m_windowRectangle);

			// Set the window style to a borderless window so the client area fills
			// the entire screen.
			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

			::SetWindowLongW(m_hWindow, GWL_STYLE, windowStyle);

			// Query the name of the nearest display device for the window.
			// This is required to set the fullscreen dimensions of the window
			// when using a multi-monitor setup.
			HMONITOR hMonitor = ::MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			::SetWindowPos(m_hWindow, HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWindow, SW_MAXIMIZE);
		}
		else
		{
			// Restore all the window decorators.
			::SetWindowLong(m_hWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(m_hWindow, HWND_NOTOPMOST,
				m_windowRectangle.left,
				m_windowRectangle.top,
				m_windowRectangle.right - m_windowRectangle.left,
				m_windowRectangle.bottom - m_windowRectangle.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWindow, SW_NORMAL);
		}
	}
}

void Window::ToggleFullscreen()
{
	SetFullscreen(!m_fullscreen);
}

void Window::Resize(uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;
}

void Window::Show() const
{
	::ShowWindow(m_hWindow, SW_SHOW);
}

HWND Window::GetWindowHandle() const
{
	return m_hWindow;
}

RECT Window::GetWindowRectangle() const
{
	RECT clientRect;
	::GetClientRect(m_hWindow, &clientRect);
	return clientRect;
}

uint32_t Window::GetWidth() const
{
	return m_width;
}

uint32_t Window::GetHeight() const
{
	return m_height;
}

void Window::RegisterWindowClass(HINSTANCE hInst, const wchar_t* pWindowClassName, WndProcCallback callback)
{
	WNDCLASSEXW windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = callback;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInst;
	windowClass.hIcon = ::LoadIcon(hInst, nullptr);
	windowClass.hCursor = ::LoadCursor(hInst, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = pWindowClassName;
	windowClass.hIconSm = ::LoadIcon(hInst, nullptr);

	static ATOM atom = ::RegisterClassExW(&windowClass);
	assert(atom > 0);
}
