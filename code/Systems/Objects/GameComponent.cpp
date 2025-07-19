/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	GameComponent::GameComponent()
		: Object()
		, m_guid()
	{ }

	GameComponent::~GameComponent()
	{ }

	void GameComponent::Update()
	{ }

	void GameComponent::Render()
	{ }

	void GameComponent::SetGuid(const Core::Guid& guid)
	{
		m_guid = guid;
	}
}
