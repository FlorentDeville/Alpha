/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <map>

#include "Editors/Property.h"

namespace Editors
{
	//Base class for a component. A component is a list of properties. A property is a key value pair.
	class Component
	{
	public:
		Component(const std::string& name);
		~Component();

		const std::string& GetName() const;

		void AddProperty(Property* property);

		int GetPropertyCount() const;

		Property* GetProperty(const std::string& name);
		const Property* GetProperty(const std::string& name) const;
		const Property* GetProperty(int index) const;

		void GetPropertyValue(const std::string& name, Systems::AssetId& id) const;
		void GetPropertyValue(const std::string& name, Core::Mat44f& m) const;

		void SetPropertyValue(const std::string& name, Systems::AssetId id);
		void SetPropertyValue(const std::string& name, const Core::Mat44f& m);

	private:
		std::string m_name;
		std::map<std::string, Property*> m_properties;
	};
}
