/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vectors.h"

namespace Rendering
{
	class Texture;
}
namespace Systems
{
	//I would prefer to keep it generic but I would need to either have dynamic material, which I don't,
	//or make some kind of specific MaterialAsset for UI
	class RenderableUI
	{
	public:
		const Rendering::Texture* m_pTexture;
		Core::Float2 m_position;
		Core::Float2 m_size;
	};
}
