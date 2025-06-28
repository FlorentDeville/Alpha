/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "OsWin/Clipboard.h"

#include <Windows.h>

namespace Os
{
	bool GetClipboardAsText(std::string& text)
	{
		// Try opening the clipboard
		if (!OpenClipboard(nullptr))
			return false;

		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == nullptr)
			return false;

		  // Lock the handle to get the actual text pointer
		char* pText = static_cast<char*>(GlobalLock(hData));
		if (pText == nullptr)
			return false;

		text = pText;

		// Release the lock
		GlobalUnlock(hData);

		// Release the clipboard
		CloseClipboard();

		return true;
	}
}
