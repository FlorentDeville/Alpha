/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Editors
{
	PropertyGridItem::PropertyGridItem()
		: m_name()
		, m_pEditingWidget(nullptr)
	{ }

	PropertyGridItem::PropertyGridItem(const std::string& name, Widgets::Widget* pEditingWidget)
		: m_name(name)
		, m_pEditingWidget(pEditingWidget)
	{ }

	const std::string& PropertyGridItem::GetName() const
	{
		return m_name;
	}

	Widgets::Widget* PropertyGridItem::GetEditingWidget() const
	{
		return m_pEditingWidget;
	}
}
