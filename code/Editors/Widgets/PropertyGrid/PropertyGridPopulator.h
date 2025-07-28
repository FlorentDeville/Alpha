/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Sid/Sid.h"

#include <map>

namespace Core
{
	class BaseArray;
}

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
	class PropertyGridItemFactory;
	class PropertyGridWidget;

	class PropertyGridPopulator
	{
	public:
		PropertyGridPopulator();
		~PropertyGridPopulator();

		void Init(PropertyGridWidget* pWidget);

		void Populate(Systems::Object* pObject);
		void Repopulate();

		void RegisterItemFactory(Core::Sid typenameSid, PropertyGridItemFactory* pFactory);

		void SetCanAddElementToArray(bool canAdd);

		void SendDataChangedEvent();

		EVENT_DECL(DataChanged, void())

	private:
		// Create properties for an instance of a class deriving from Object.
		void CreatePropertiesForObject(Systems::Object* pObject, int depth);

		// Recursively goes up the inheritance hierarchy of an Object to display all its properties.
		void CreatePropertiesForObjectParentClass(Systems::Object* pObject, const Systems::TypeDescriptor* pType, int depth);

		void CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pArrayPtr, int depth);
		Widgets::Widget* CreateWidgetForPODField(const Systems::TypeDescriptor* pFieldType, void* pData, bool readOnly);

		// Create properties for all members in the TypeDescriptor. It doesn't handle base classes.
		void CreatePropertiesForTypeMembers(const Systems::TypeDescriptor* pFieldType, void* pData, int depth);

		void CreateArrayAddElementButton(const Systems::FieldDescriptor& member, void* pMemberPtr);

		PropertyGridItemFactory* GetFactory(Core::Sid typeSid) const;

		Widgets::Widget* CreateArrayItemName(Core::BaseArray* pArray, int elementIndex, bool elementIsObject, void* pElement);

		PropertyGridWidget* m_pPropertyGridWidget;

		//Map of type name sid to item factory
		std::map<Core::Sid, PropertyGridItemFactory*> m_factories;

		//The object currently being displayed
		Systems::Object* m_pObject;

		bool m_canAddElementToArray;
	};
}
