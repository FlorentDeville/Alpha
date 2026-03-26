/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Widgets
{
	// Defines how to compute the size of a widget:
	// DEFAULT	: fixed size.
	// STRETCH	: stretch the widget to take all its parent's space.
	// FIT		: take the size of its content.
	enum SIZE_STYLE
	{
		HSIZE_DEFAULT = 0x00000001,
		VSIZE_DEFAULT = 0x00000010,

		HSIZE_STRETCH = 0x00000100,
		VSIZE_STRETCH = 0x00001000,

		HSIZE_FIT = 0x00010000,
		VSIZE_FIT = 0x00100000,

		DEFAULT = HSIZE_DEFAULT | VSIZE_DEFAULT,
		STRETCH = HSIZE_STRETCH | VSIZE_STRETCH,
		FIT = HSIZE_FIT | VSIZE_FIT,

		HSTRETCH_VFIT = HSIZE_STRETCH | VSIZE_FIT
	};
}
