/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Entity.h"

#include "Editors/LevelEditor/Component.h"

namespace Editors
{
	Entity::Entity(const std::string& name)
		: m_components()
		, m_name(name)
	{}

	Entity::~Entity()
	{
		for (const std::pair<std::string, Component*>& pair : m_components)
			delete pair.second;

		m_components.clear();
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

	int Entity::GetComponentCount() const
	{
		return static_cast<int>(m_components.size());
	}

	const std::string& Entity::GetName() const
	{
		return m_name;
	}
}
