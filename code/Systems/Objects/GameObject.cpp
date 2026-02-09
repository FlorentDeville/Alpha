/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/GameObject.h"

namespace Systems
{
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

	void GameObject::Update()
	{
		for (GameComponent* pComponent : m_components)
			pComponent->Update();
	}

	void GameObject::UpdateTransform()
	{
		m_transform.Update();
	}

	void GameObject::Render()
	{
		for (GameComponent* pComponent : m_components)
			pComponent->Render();
	}

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

	void GameObject::PostLoad()
	{
		Object::PostLoad();

		m_transform.SetOwner(this);

		for (GameComponent* pComponent : m_components)
			pComponent->SetOwner(this);
	}

	GameObject* CreateNewGameObject(const TypeDescriptor* pType)
	{
		GameObject* pObject = static_cast<GameObject*>(CreateObject(pType));
		pObject->SetGuid(Core::Guid::GenerateNewGuid());
		pObject->GetTransform().SetGuid(Core::Guid::GenerateNewGuid());

		return pObject;
	}
}
