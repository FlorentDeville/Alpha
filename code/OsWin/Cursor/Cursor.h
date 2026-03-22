/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Os
{
	enum class CursorId : int
	{
		Unknown = -1,
		
		Arrow, 
		IBeam,
		Wait,
		Cross,
		UpArrow,
		ResizeDiagonalNWSE,
		ResizeDiagonalNESW,
		ResizeHorizontal,
		ResizeVertical,
		Move,

		Count
	};

	void SetCursor(CursorId id);
}
