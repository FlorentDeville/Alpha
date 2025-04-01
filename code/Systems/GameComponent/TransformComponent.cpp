/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/TransformComponent.h"

namespace Systems
{
	TransformComponent::TransformComponent()
		: GameComponent()
		, m_localTransform()
	{ }

	TransformComponent::~TransformComponent()
	{ }
}
