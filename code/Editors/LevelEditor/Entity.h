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
		Component* GetComponent(int index);

		int GetComponentCount() const;

		const std::string& GetName() const;

	private:
		std::string m_name;
		std::map<std::string, Component*> m_components;
	};
}
