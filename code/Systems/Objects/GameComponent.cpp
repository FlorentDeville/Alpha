/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	GameComponent::GameComponent()
		: Object()
		, m_guid()
		, m_enabled(true)
		, m_pOwnerGo(nullptr)
	{ }

	GameComponent::~GameComponent()
	{ }

	void GameComponent::OnStart(GameContext* pWorld)
	{ }

	void GameComponent::OnStartGame()
	{ }

	void GameComponent::Update(float dt)
	{ }

	void GameComponent::OnDestroy(GameContext* pWorld)
	{ }

	void GameComponent::OnDestroyGame()
	{ }

	const Core::Guid& GameComponent::GetGuid() const
	{
		return m_guid;
	}

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

	GameObject* GameComponent::GetOwner()
	{
		return m_pOwnerGo;
	}

	bool GameComponent::IsEnabled() const
	{
		return m_enabled;
	}

	void GameComponent::SetEnabled(bool isEnabled)
	{
		m_enabled = isEnabled;
	}

	GameComponent* CreateNewGameComponent(const Core::TypeDescriptor* pType)
	{
		GameComponent* ptr = static_cast<GameComponent*>(CreateObject(pType));
		ptr->SetGuid(Core::Guid::GenerateNewGuid());
		return ptr;
	}
}
