/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "OsWin/Cursor/Cursor.h"

#include <Windows.h>

namespace OsWin
{
	LPCSTR winCursorTable[static_cast<int>(CursorId::Count)] =
	{
		IDC_ARROW,			//CursorId::Arrow
		IDC_IBEAM,			//CursorId::IBeam
		IDC_WAIT,			//CursorId::Wait
		IDC_CROSS,			//CursorId::Cross
		IDC_UPARROW,		//CursorId::UpArrow
		IDC_SIZENWSE,		//CursorId::ResizeDiagonalNWSE
		IDC_SIZENESW,		//CursorId::ResizeDiagonalNESW
		IDC_SIZEWE,			//CursorId::ResizeHorizontal
		IDC_SIZENS,			//CursorId::ResizeVertical
		IDC_SIZEALL			//CursorId::Move
	};

	
	void SetCursor(CursorId id)
	{
		LPCSTR winCursor = winCursorTable[static_cast<int>(id)];
		HCURSOR cursor = LoadCursor(NULL, winCursor);
		SetCursor(cursor);
	}
}
