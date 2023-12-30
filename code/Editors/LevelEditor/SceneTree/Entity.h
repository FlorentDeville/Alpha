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
	class Property;

	class Entity : public Node
	{
	public:
		Entity(const std::string& name);
		~Entity();

		Entity* ToEntity() override;
		const Entity* ToConstEntity() const override;

		void AddComponent(Component* component);

		const Component* GetComponent(const std::string& name) const;
		Component* GetComponent(const std::string& name);
		const Component* GetComponent(int index) const;
		Component* GetComponent(int index);

		Property* GetProperty(const std::string& componentName, const std::string& propertyName);
		const Property* GetConstProperty(const std::string& componentName, const std::string& propertyName) const;

		int GetComponentCount() const;

		const std::string& GetName() const;

		virtual Core::Mat44f ComputeWs() const;
		virtual Core::Mat44f ComputeParentWs() const;

		virtual const Core::Mat44f& GetLs() const;

		virtual void SetLs(const Core::Mat44f& txLs);
		void SetName(const std::string& name);

	private:
		std::string m_name;
		std::map<std::string, Component*> m_components;

		static Core::Mat44f s_default;
	};
}
