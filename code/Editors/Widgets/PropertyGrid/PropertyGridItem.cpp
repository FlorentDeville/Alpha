/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

#include "Widgets/Label.h"

namespace Editors
{
	PropertyGridItem::PropertyGridItem()
		: m_pEditingWidget(nullptr)
		, m_pNameWidget(nullptr)
	{ }

	PropertyGridItem::PropertyGridItem(const std::string& name, Widgets::Widget* pEditingWidget)
		: m_pEditingWidget(pEditingWidget)
	{
		m_pNameWidget = new Widgets::Label(name);
	}

	PropertyGridItem::PropertyGridItem(Widgets::Widget* pNameWidget, Widgets::Widget* pEditingWidget)
		: m_pNameWidget(pNameWidget)
		, m_pEditingWidget(pEditingWidget)
	{ }

	Widgets::Widget* PropertyGridItem::GetNameWidget() const
	{
		return m_pNameWidget;
	}

	Widgets::Widget* PropertyGridItem::GetEditingWidget() const
	{
		return m_pEditingWidget;
	}
}
