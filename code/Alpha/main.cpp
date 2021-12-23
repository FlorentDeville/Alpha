/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>
#include <cstdint>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void RegisterWindowClass(HINSTANCE hInst, const wchar_t* pWindowClassName)
{
	WNDCLASSEXW windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
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

HWND CreateCustomWindow(const wchar_t* pWindowClassName, HINSTANCE hInst, const wchar_t* pWindowTitle, uint32_t width, uint32_t height)
{
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = static_cast<LONG>(width);
	windowRect.bottom = static_cast<LONG>(height);
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	HWND hWnd = ::CreateWindowExW(0, pWindowClassName, pWindowTitle, WS_OVERLAPPEDWINDOW, windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, hInst, nullptr);
	assert(hWnd && "Failed to create window");
	return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				::PostQuitMessage(0);
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

	default:
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	const wchar_t* pWindowClassName = L"DX12WindowClass";
	RegisterWindowClass(hInstance, pWindowClassName);
	HWND hWnd = CreateCustomWindow(pWindowClassName, hInstance, L"Alpha", 1080, 789);

	::ShowWindow(hWnd, SW_SHOW);

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return 0;
}
