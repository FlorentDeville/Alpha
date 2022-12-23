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
		Entity();
		~Entity();

		void AddComponent(Component* component);

		const Component* GetComponent(const std::string& name) const;

	private:
		std::map<std::string, Component*> m_components;
	};
}
