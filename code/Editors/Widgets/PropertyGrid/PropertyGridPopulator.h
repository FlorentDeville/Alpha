/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

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

	private:
		// Create properties for an instance of a class deriving from Object.
		void CreatePropertiesForObject(Systems::Object* pObject, int depth);

		// Recursively goes up the inheritance hierarchy of an Object to display all its properties.
		void CreatePropertiesForObjectParentClass(Systems::Object* pObject, const Systems::TypeDescriptor* pType, int depth);

		void CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pObj, int depth);

		void CreatePropertiesForSingleArrayElement(const Systems::FieldDescriptor* pField, void* pObj, int depth, uint32_t index);
		
		PropertyGridItem* CreatePropertyItemForPODField(const Systems::FieldDescriptor* pField, void* pObj, uint32_t indexElement);

		// Create properties for all members in the TypeDescriptor. It doesn't handle base classes.
		void CreatePropertiesForTypeMembers(const Systems::TypeDescriptor* pFieldType, void* pData, int depth);

		PropertyGridItemFactory* GetFactory(Core::Sid typeSid) const;

		void ObjectWatcherCallback(Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index);

		PropertyGridWidget* m_pPropertyGridWidget;

		//Map of type name sid to item factory
		std::map<Core::Sid, PropertyGridItemFactory*> m_factories;

		std::map<const Systems::Object*, Core::CallbackId> m_watcherCallbackIds;

		//The object currently being displayed
		Systems::Object* m_pObject;

		bool m_canAddElementToArray;
	};
}
