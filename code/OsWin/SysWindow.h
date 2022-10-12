/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

#include <cstdint>
#include <string>

typedef LRESULT (*WndProcCallback)(HWND, UINT, WPARAM, LPARAM);

class SysWindow
{
public:
	SysWindow();
	virtual ~SysWindow();

	bool Create(const char* pWindowClassName, const char* pWindowTitle, uint32_t width, uint32_t height, HINSTANCE hInstance);

	void SetFullscreen(bool fullscreen);
	void ToggleFullscreen();

	void Resize(uint32_t width, uint32_t height);

	void Show() const;

	HWND GetWindowHandle() const;
	RECT GetWindowRectangle() const;
	RECT GetClientRectangle() const;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	static void RegisterWindowClass(HINSTANCE hInst, const char* pWindowClassName, WndProcCallback callback, const std::string& iconFilename);

private:
	HWND m_hWindow;
	bool m_fullscreen;

	uint32_t m_width;
	uint32_t m_height;

	RECT m_windowRectangle;	//save the window dimension when going to full screen so it can be restored when going back to windowed mode.
};
