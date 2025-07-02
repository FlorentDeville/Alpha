/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	class TypeDescriptor;
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

		virtual void CreateItems(const Systems::TypeDescriptor* pType, void* pData, int depth) = 0;

	protected:
		PropertyGridWidget* m_pPropertyGridWidget;
		PropertyGridPopulator* m_pPopulator;
	};
}

