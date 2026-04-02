/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/ResourceFormat.h"

#include<dxgiformat.h>

namespace Rendering
{
	namespace Internal
	{
		DXGI_FORMAT GetDx12ResourceFormat(ResourceFormat format);
	}
}