/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Entity.h"

#include "Editors/LevelEditor/Component.h"

namespace Editors
{
	Entity::Entity()
		: m_components()
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
}
