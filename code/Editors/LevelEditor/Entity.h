/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <map>
#include <string>

namespace Editors
{
	class Component;

	class Entity
	{
	public:
		Entity(const std::string& name);
		~Entity();

		void AddComponent(Component* component);

		const Component* GetComponent(const std::string& name) const;
		const Component* GetComponent(int index) const;
		
		int GetComponentCount() const;

	private:
		std::string m_name;
		std::map<std::string, Component*> m_components;
	};
}
