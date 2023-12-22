/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/LevelEditor/SceneTree/Node.h"

#include <map>
#include <string>

namespace Core
{
	class Mat44f;
}

namespace Editors
{
	class Component;

	class Entity : public Node
	{
	public:
		Entity(const std::string& name);
		~Entity();

		Entity* ToEntity() override;
		const Entity* ToConstEntity() const override;

		void AddComponent(Component* component);

		const Component* GetComponent(const std::string& name) const;
		const Component* GetComponent(int index) const;
		Component* GetComponent(int index);

		int GetComponentCount() const;

		const std::string& GetName() const;

		virtual Core::Mat44f ComputeWs() const;
		virtual Core::Mat44f ComputeParentWs() const;

	private:
		std::string m_name;
		std::map<std::string, Component*> m_components;
	};
}
