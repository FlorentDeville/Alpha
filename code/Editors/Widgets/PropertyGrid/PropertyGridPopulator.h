/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Sid/Sid.h"

#include <map>

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

		void RegisterItemFactory(Core::Sid typenameSid, PropertyGridItemFactory* pFactory);

		void SendDataChangedEvent();

		EVENT_DECL(DataChanged, void())

	private:
		void CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pArrayPtr, int depth);
		Widgets::Widget* CreateWidgetForPODField(const Systems::TypeDescriptor* pFieldType, void* pData, bool readOnly);
		void CreatePropertiesForClassMember(const Systems::TypeDescriptor* pFieldType, void* pData, int depth);

		PropertyGridItemFactory* GetFactory(Core::Sid typeSid) const;

		PropertyGridWidget* m_pPropertyGridWidget;

		//Map of type name sid to item factory
		std::map<Core::Sid, PropertyGridItemFactory*> m_factories;
	};
}
