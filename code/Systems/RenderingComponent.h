/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Material/MaterialId.h"
#include "Rendering/Mesh/MeshId.h"

namespace Systems
{
	class RenderingComponent
	{
	public:
		Rendering::MeshId m_meshId;
		Rendering::MaterialId m_materialId;
	};
}
