/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridItemFactory.h"

namespace Editors
{
	PropertyGridItemFactory::PropertyGridItemFactory()
		: m_pPropertyGridWidget(nullptr)
		, m_pPopulator(nullptr)
	{ }

	PropertyGridItemFactory::~PropertyGridItemFactory()
	{ }

	void PropertyGridItemFactory::Init(PropertyGridWidget* pWidget, PropertyGridPopulator* pPopulator)
	{
		m_pPropertyGridWidget = pWidget;
		m_pPopulator = pPopulator;
	}
}
