/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

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

		Widgets::Widget* GetNameWidget() const;
		Widgets::Widget* GetEditingWidget() const;

	private:
		//The widget is not owned by the property
		Widgets::Widget* m_pEditingWidget;
		Widgets::Widget* m_pNameWidget;
	};
}
