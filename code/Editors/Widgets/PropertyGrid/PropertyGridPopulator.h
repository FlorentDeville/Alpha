/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

namespace Systems
{
	class FieldDescriptor;
	class Object;
	class TypeDescriptor;
}

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class PropertyGridWidget;

	//This class should be turned into a model
	class PropertyGridPopulator
	{
	public:
		PropertyGridPopulator();
		~PropertyGridPopulator();

		void Populate(PropertyGridWidget* pWidget, Systems::Object* pObject);

		EVENT_DECL(DataChanged, void())

	private:
		void CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pArrayPtr);
		Widgets::Widget* CreateWidgetForPODField(const Systems::TypeDescriptor* pFieldType, void* pData);
		void CreatePropertiesForClassMember(const Systems::TypeDescriptor* pFieldType, void* pData);

		PropertyGridWidget* m_pPropertyGridWidget;
	};
}
