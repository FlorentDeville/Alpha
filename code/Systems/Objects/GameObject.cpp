/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Objects/GameObject.h"

namespace Systems
{
	GameObject::GameObject()
		: GameObject("")
	{ }

	GameObject::GameObject(const std::string& name)
		: Object()
		, m_name(name)
	{
		Systems::CreateObjectInPlace(&m_transform);
		m_transform.SetOwner(this);
	}

	GameObject::~GameObject()
	{
		for (GameComponent* pComponent : m_components)
			delete pComponent;

		m_components.Clear();
	}

	void GameObject::OnStart(GameContext* pWorld)
	{
		for (GameComponent* pComponent : m_components)
			pComponent->OnStart(pWorld);
	}

	void GameObject::OnStartGame()
	{
		for (GameComponent* pComponent : m_components)
			pComponent->OnStartGame();
	}

	void GameObject::Update(float dt)
	{
		for (GameComponent* pComponent : m_components)
			pComponent->Update(dt);
	}

	void GameObject::PostUpdate()
	{ }

	void GameObject::OnDestroy(GameContext* pWorld)
	{
		for (GameComponent* pComponent : m_components)
			pComponent->OnDestroy(pWorld);
	}

	void GameObject::OnDestroyGame()
	{
		for (GameComponent* pComponent : m_components)
			pComponent->OnDestroyGame();
	}

	void GameObject::HandleMessage(const GameMessage& msg)
	{ }

	void GameObject::SetGuid(const Core::Guid& guid)
	{
		m_guid = guid;
	}

	const Core::Guid& GameObject::GetGuid() const
	{
		return m_guid;
	}

	void GameObject::SetName(const std::string& name)
	{
		m_name = name;
	}

	const std::string& GameObject::GetName() const
	{
		return m_name;
	}

	TransformComponent& GameObject::GetTransform()
	{
		return m_transform;
	}

	const TransformComponent& GameObject::GetTransform() const
	{
		return m_transform;
	}

	const Core::Array<GameComponent*>& GameObject::GetComponents() const
	{
		return m_components;
	}

	void GameObject::AddComponent(GameComponent* pComponent)
	{
		m_components.PushBack(pComponent);
		pComponent->SetOwner(this);
	}

	const GameComponent* GameObject::FindComponent(const Core::Guid& guid) const
	{
		for (const GameComponent* pComponent : m_components)
		{
			if (pComponent->GetGuid() == guid)
				return pComponent;
		}

		return nullptr;
	}

	GameComponent* GameObject::FindComponent(const Core::Guid& guid)
	{
		for (GameComponent* pComponent : m_components)
		{
			if (pComponent->GetGuid() == guid)
				return pComponent;
		}

		return nullptr;
	}

	void GameObject::PostLoad()
	{
		Object::PostLoad();

		m_transform.SetOwner(this);

		for (GameComponent* pComponent : m_components)
			pComponent->SetOwner(this);
	}

	GameObject* CreateNewGameObject(const Core::TypeDescriptor* pType)
	{
		GameObject* pObject = static_cast<GameObject*>(CreateObject(pType));
		pObject->SetGuid(Core::Guid::GenerateNewGuid());
		pObject->GetTransform().SetGuid(Core::Guid::GenerateNewGuid());

		return pObject;
	}
}
