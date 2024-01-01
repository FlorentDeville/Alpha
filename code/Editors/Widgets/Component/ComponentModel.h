/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/LevelEditor/Component.h"
#include "Editors/Property.h"

#include "Systems/Assets/AssetType.h"

#include "Widgets/WidgetEventMacro.h"

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

		template <class T> const T& GetPropertyValue(int row) const
		{
			Property* pProperty = m_pComponent->GetProperty(row);
			return pProperty->GetValue<T>();
		}

		std::string ToString(Systems::AssetId id) const;

		void SetPropertyValue(int row, const Core::Mat44f& value);
		void SetPropertyValue(int row, Systems::AssetId value, Systems::AssetType type);

		BaseModel* GetModel(int row) const;

		EVENT_DECL(PropertyValueChanged, void(int row))

	private:
		Component* m_pComponent;

		std::vector<BaseModel*> m_modelsArray;

		std::vector<Core::CallbackId> m_cidPropertyOnValueChanged;

		static std::string s_defaultValue;
	};
}
