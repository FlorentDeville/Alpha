/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Systems
{
	class FieldDescriptor;
	class Object;
}

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	// Represents a single property in a property grid.
	class PropertyGridItem
	{
	public:
		PropertyGridItem();
		PropertyGridItem(const std::string& name, Widgets::Widget* pEditingWidget);
		PropertyGridItem(Widgets::Widget* pNameWidget, Widgets::Widget* pEditingWidget);
		PropertyGridItem(Systems::Object* pObj, Systems::FieldDescriptor* pField, uint32_t index);

		Widgets::Widget* GetNameWidget() const;
		Widgets::Widget* GetEditingWidget() const;

		// Create the widgets rendered by the PropertyGridWidget
		virtual void CreateWidgets();

		// Update the value displayed in the widget
		virtual void UpdateValue();

		// Check if this item corresponds to a certain field for an given object
		bool IsField(const Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index) const;

	protected:

		//The widgets are not owned by the item.
		Widgets::Widget* m_pEditingWidget;
		Widgets::Widget* m_pNameWidget;

		//Object, field and optional index this item represents.
		Systems::Object* m_pObj;
		Systems::FieldDescriptor* m_pField;
		uint32_t m_index;

		Widgets::Widget* CreateDefaultItemLabel();

		Widgets::Widget* CreatePodItemLabel();
		Widgets::Widget* CreateArrayItemLabel();
	};
}
