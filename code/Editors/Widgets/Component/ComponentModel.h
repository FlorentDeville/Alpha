/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>
#include <string>

namespace Editors
{
	class BaseModel;
	class Component;
	enum PropertyType;

	class ComponentModel
	{
	public:
		ComponentModel(Component* pComponent);
		~ComponentModel();

		int GetPropertyCount() const;

		const std::string& GetPropertyName(int row) const;
		PropertyType GetPropertyType(int row) const;

		BaseModel* GetModel(int row) const;

	private:
		Component* m_pComponent;

		std::vector<BaseModel*> m_modelsArray;

		static std::string s_defaultValue;
	};
}
