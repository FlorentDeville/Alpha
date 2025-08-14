/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Sid/Sid.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

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
	class PropertyGridItem;
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

		void AddPropertyGridItem(PropertyGridItem* pItem);

	private:
		// Create properties for an instance of a class deriving from Object.
		void CreatePropertiesForObject(Systems::Object* pObject);

		// Recursively goes up the inheritance hierarchy of an Object to display all its properties.
		void CreatePropertiesForObjectParentClass(Systems::Object* pObject, const Systems::TypeDescriptor* pType);

		void CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pObj);

		void CreatePropertiesForSingleArrayElement(const Systems::FieldDescriptor* pField, void* pObj, uint32_t index);
		
		PropertyGridItem* CreatePropertyItemForPODField(const Systems::FieldDescriptor* pField, void* pObj, uint32_t indexElement);

		// Create properties for all members in the TypeDescriptor. It doesn't handle base classes.
		void CreatePropertiesForTypeMembers(const Systems::TypeDescriptor* pFieldType, void* pData);

		void Internal_AddPropertyGridItem(PropertyGridItem* pItem);

		// Overload to add a property item at the given index in its parent
		void Internal_AddPropertyGridItem(PropertyGridItem* pItem, uint32_t index);

		//This function does not delete pItemToDelete. It deletes recursively all the children of pItemToDelete
		void DeletePropertyGridItemRecursively(PropertyGridItem* pItemToDelete);

		PropertyGridItemFactory* GetFactory(Core::Sid typeSid) const;

		void ObjectWatcherCallback(Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index);

		PropertyGridWidget* m_pPropertyGridWidget;

		//Map of type name sid to item factory
		std::map<Core::Sid, PropertyGridItemFactory*> m_factories;

		Core::Array<ObjectWatcherCallbackId> m_watcherCallbackIds;

		//Tree of property items mapping parent item to a list of children items
		std::map<const PropertyGridItem*, Core::Array<PropertyGridItem*>> m_propertyItemsTree;

		//Map from an item to its parent. The parent is ullptr if it's a root item.
		std::map<const PropertyGridItem*, PropertyGridItem*> m_propertyItemParent;

		// Map of property item to the depth of the property.
		std::map< const PropertyGridItem*, uint32_t> m_propertyItemDepth;

		//The object currently being displayed
		Systems::Object* m_pObject;

		bool m_canAddElementToArray;

		struct ParentItemContext
		{
			PropertyGridItem* m_pParent;
			int depth;
		};

		class ParentItemContextScope
		{
		public:
			ParentItemContextScope(PropertyGridItem* m_pParent, PropertyGridPopulator* pPopulator);
			~ParentItemContextScope();

		private:
			PropertyGridPopulator* m_pPopulator;
			ParentItemContext m_previous;
		};

		ParentItemContext m_parentItemContext;
	};
}
