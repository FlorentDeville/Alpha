/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Systems
{
	class FieldDescriptor;
}

namespace Widgets
{
	//class Widget;
}

namespace Editors
{
	class PropertyGridPopulator;
	class PropertyGridWidget;

	// Class to create custom widgets for a given type in a property grid.
	class PropertyGridItemFactory
	{
	public:
		PropertyGridItemFactory();
		virtual ~PropertyGridItemFactory();

		void Init(PropertyGridWidget* pWidget, PropertyGridPopulator* pPopulator);

		virtual void CreateItems(void* pObj, const Systems::FieldDescriptor* pField, uint32_t index) = 0;

	protected:
		PropertyGridWidget* m_pPropertyGridWidget;
		PropertyGridPopulator* m_pPopulator;
	};
}

