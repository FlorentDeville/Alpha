/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Objects/GameComponent.h"
#include "Systems/Assets/AssetId.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, StaticMeshComponent)
	class StaticMeshComponent : public GameComponent
	{
	public:
		StaticMeshComponent();
		~StaticMeshComponent();

	private:

		AssetId m_meshId;
		AssetId m_materialId;

		START_REFLECTION(Systems::StaticMeshComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_meshId)
			ADD_FIELD(m_materialId)
		END_REFLECTION()
	};
}
