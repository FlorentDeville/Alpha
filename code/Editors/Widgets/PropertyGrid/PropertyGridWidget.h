/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

#include <vector>

namespace Widgets
{
	class Layout;
}

namespace Editors
{
	class PropertyGridItem;

	class PropertyGridWidget : public Widgets::Container
	{
	public:
		PropertyGridWidget();
		~PropertyGridWidget();

		void AddProperty(PropertyGridItem* pProperty, int depth = 0);

		void ClearAllItems();

	private:
		std::vector<PropertyGridItem*> m_properties;

		Widgets::Layout* m_pInternalLayout;

		//Width of the name column
		uint32_t m_nameColumnWidth;
		uint32_t m_rowHeight;
	};
}
