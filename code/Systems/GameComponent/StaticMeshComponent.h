/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Objects/GameComponent.h"


namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, StaticMeshComponent)
	class StaticMeshComponent : public GameComponent
	{
	public:
		StaticMeshComponent();
		~StaticMeshComponent();

	private:

		NewAssetId m_meshId;
		NewAssetId m_materialId;

		START_REFLECTION(Systems::StaticMeshComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_meshId)
			ADD_FIELD(m_materialId)
		END_REFLECTION()
	};
}
