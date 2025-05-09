/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Editors
{
	PropertyGridWidget::PropertyGridWidget()
		: Widgets::Container()
	{
		m_pInternalLayout = new Widgets::Layout();
		m_pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		AddWidget(m_pInternalLayout);
	}

	PropertyGridWidget::~PropertyGridWidget()
	{
		for (PropertyGridItem* pProperty : m_properties)
			delete pProperty;
	}

	void PropertyGridWidget::AddProperty(PropertyGridItem* pProperty)
	{
		m_properties.push_back(pProperty);

		Widgets::Layout* pPropertyLayout = new Widgets::Layout();
		pPropertyLayout->SetDirection(Widgets::Layout::Direction::Horizontal);

		m_pInternalLayout->AddWidget(pPropertyLayout);

		pPropertyLayout->AddWidget(new Widgets::Label(pProperty->GetName()));
		pPropertyLayout->AddWidget(pProperty->GetEditingWidget());
	}
}
