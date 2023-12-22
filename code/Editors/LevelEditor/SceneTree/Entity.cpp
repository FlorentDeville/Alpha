/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/SceneTree/Entity.h"

#include "Editors/LevelEditor/Component.h"

namespace Editors
{
	Entity::Entity(const std::string& name)
		: Node()
		, m_components()
		, m_name(name)
	{}

	Entity::~Entity()
	{
		for (const std::pair<std::string, Component*>& pair : m_components)
			delete pair.second;

		m_components.clear();
	}

	Entity* Entity::ToEntity()
	{
		return this;
	}

	const Entity* Entity::ToConstEntity() const
	{
		return this;
	}

	void Entity::AddComponent(Component* pComponent)
	{
		m_components[pComponent->GetName()] = pComponent;
	}

	const Component* Entity::GetComponent(const std::string& name) const
	{
		std::map<std::string, Component*>::const_iterator it = m_components.find(name);
		if (it == m_components.cend())
			return nullptr;

		return it->second;
	}

	const Component* Entity::GetComponent(int index) const
	{
		std::map<std::string, Component*>::const_iterator it = m_components.begin();
		for (int ii = 0; ii < index; ++ii)
			++it;

		return it->second;
	}

	Component* Entity::GetComponent(int index)
	{
		std::map<std::string, Component*>::const_iterator it = m_components.begin();
		for (int ii = 0; ii < index; ++ii)
			++it;

		return it->second;
	}

	int Entity::GetComponentCount() const
	{
		return static_cast<int>(m_components.size());
	}

	const std::string& Entity::GetName() const
	{
		return m_name;
	}

	Core::Mat44f Entity::ComputeWs() const
	{
		Core::Mat44f lsTransform;
		lsTransform.SetIdentity();
		if (const Component* pComponent = GetComponent("Transform"))
		{
			if (const Property* pProperty = pComponent->GetProperty("Local"))
			{
				const PropertyValueMat44f& value = static_cast<const PropertyValueMat44f&>(pProperty->GetValue());
				lsTransform = value.Get();
			}
		}

		const Node* pParent = GetConstParent();
		if (!pParent)
			return lsTransform;

		Core::Mat44f parentWs = ComputeParentWs();
		Core::Mat44f ws = lsTransform * parentWs;
		return ws;
	}

	Core::Mat44f Entity::ComputeParentWs() const
	{
		Core::Mat44f parentWs;
		parentWs.SetIdentity();

		const Node* pParentNode = GetConstParent();
		if (!pParentNode)
			return parentWs;

		const Entity* pParentEntity = pParentNode->ToConstEntity();
		if (!pParentEntity)
			return parentWs;

		return pParentEntity->ComputeWs();
	}

}
