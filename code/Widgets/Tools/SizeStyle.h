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
		HSIZE_DEFAULT	= 1 << 0,
		VSIZE_DEFAULT	= 1 << 1,

		HSIZE_STRETCH	= 1 << 2,
		VSIZE_STRETCH	= 1 << 3,

		HSIZE_FIT		= 1 << 4,
		VSIZE_FIT		= 1 << 5,

		DEFAULT = HSIZE_DEFAULT | VSIZE_DEFAULT,
		STRETCH = HSIZE_STRETCH | VSIZE_STRETCH,
		FIT = HSIZE_FIT | VSIZE_FIT,

		HDEFAULT_VSTRETCH = HSIZE_DEFAULT | VSIZE_STRETCH,
		HDEFAULT_VFIT = HSIZE_DEFAULT | VSIZE_FIT,

		HSTRETCH_VDEFAULT = HSIZE_STRETCH | VSIZE_DEFAULT,
		HSTRETCH_VFIT = HSIZE_STRETCH | VSIZE_FIT,

		HFIT_VDEFAULT = HSIZE_FIT | VSIZE_DEFAULT,
		HFIT_VSTRETCH = HSIZE_FIT | VSIZE_STRETCH,
	};

	inline SIZE_STYLE operator|(SIZE_STYLE a, SIZE_STYLE b)
	{
		return static_cast<SIZE_STYLE>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline bool operator&(SIZE_STYLE a, SIZE_STYLE b)
	{
		return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
	}
}
