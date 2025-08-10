/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/FieldDescriptor.h"

#include <string>

namespace Systems
{
	class FieldDescriptor;
	class Object;
}

namespace Widgets
{
	class Button;
	class Label;
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
		PropertyGridItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);

		Widgets::Widget* GetNameWidget() const;
		Widgets::Widget* GetEditingWidget() const;

		// Create the widgets rendered by the PropertyGridWidget
		virtual void CreateWidgets();

		// Update the value displayed in the widget
		virtual void UpdateValue();

		// Check if this item corresponds to a certain field for an given object
		bool IsField(const Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index) const;

		//Change the index of the element this item represents. Use this when deleting/inserting an element in an array
		virtual void ChangeIndex(uint32_t newIndex);

		template<typename T> const T* GetDataPtr() const;

		uint32_t GetIndex() const;

	protected:

		//The widgets are not owned by the item.
		Widgets::Widget* m_pEditingWidget;
		Widgets::Widget* m_pNameWidget;

		//Label displaying the name of the field
		Widgets::Label* m_pFieldName;

		Widgets::Button* m_pDeleteButton;

		//Object, field and optional index this item represents.
		Systems::Object* m_pObj;
		const Systems::FieldDescriptor* m_pField;
		uint32_t m_index;

		Widgets::Widget* CreateDefaultItemLabel();

		Widgets::Widget* CreatePodItemLabel();
		Widgets::Widget* CreateArrayItemLabel();

		void ArrayItemLabel_Init();
	};

	template<typename T> const T* PropertyGridItem::GetDataPtr() const
	{
		if (m_pField->GetType()->IsContainer())
		{
			Core::BaseArray* pArray = m_pField->GetDataPtr<Core::BaseArray>(m_pObj);
			const void* pElement = pArray->GetConstElement(m_index);
			return reinterpret_cast<const T*>(pElement);
		}
		else
		{
			return m_pField->GetDataPtr<const T>(m_pObj);
		}
	}
}
