/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	GameComponent::GameComponent()
		: Object()
		, m_guid()
		, m_pOwnerGo(nullptr)
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

	void GameComponent::SetOwner(GameObject* pOwnerGo)
	{
		m_pOwnerGo = pOwnerGo;
	}

	const GameObject* GameComponent::GetOwner() const
	{
		return m_pOwnerGo;
	}

	GameComponent* CreateNewGameComponent(const TypeDescriptor* pType)
	{
		GameComponent* ptr = static_cast<GameComponent*>(CreateObject(pType));
		ptr->SetGuid(Core::Guid::GenerateNewGuid());
		return ptr;
	}
}
