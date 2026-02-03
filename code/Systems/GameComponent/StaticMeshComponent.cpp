/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/StaticMeshComponent.h"

namespace Systems
{
	StaticMeshComponent::StaticMeshComponent()
		: GameComponent()
		, m_mesh()
		, m_material()
	{ }

	StaticMeshComponent::~StaticMeshComponent()
	{ }
}
