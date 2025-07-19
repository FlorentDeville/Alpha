/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	GameComponent::GameComponent()
		: Object()
		, m_id()
	{ }

	GameComponent::~GameComponent()
	{ }

	void GameComponent::Update()
	{ }

	void GameComponent::Render()
	{ }
}
