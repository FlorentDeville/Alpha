/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"

namespace Rendering
{
	class PerObjectCBuffer;
	class PerFrameCBuffer;
}

namespace Systems
{	
	class MaterialRendering
	{
	public:
		static void Bind(const MaterialAsset& material, const Rendering::PerObjectCBuffer& perObjectCBuffer,
			const Rendering::PerFrameCBuffer& perFrameCBuffer);
	};
}
