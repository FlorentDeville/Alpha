/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

namespace Rendering
{
	class LightsCBuffer;
	class PerObjectCBuffer;
	class PerFrameCBuffer;
}

namespace Systems
{	
	class MaterialRendering
	{
	public:
		static void Bind(const MaterialAsset& material, const Rendering::PerObjectCBuffer& perObjectCBuffer, 
			const Rendering::PerFrameCBuffer& perFrameCBuffer,
			const Rendering::LightsCBuffer& lights);

		static void Bind(const MaterialInstanceAsset& materialInstance, const Rendering::PerObjectCBuffer& perObjectCBuffer,
			const Rendering::PerFrameCBuffer& perFrameCBuffer,
			const Rendering::LightsCBuffer& lights);
	};
}
