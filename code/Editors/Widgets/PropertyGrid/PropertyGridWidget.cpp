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
		m_pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		AddWidget(m_pInternalLayout);
	}

	PropertyGridWidget::~PropertyGridWidget()
	{
		ClearAllItems();
	}

	void PropertyGridWidget::AddProperty(PropertyGridItem* pProperty)
	{
		m_properties.push_back(pProperty);

		Widgets::Layout* pPropertyLayout = new Widgets::Layout();
		pPropertyLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pPropertyLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::HSIZE_STRETCH | Widgets::Widget::SIZE_STYLE::VSIZE_FIT);

		m_pInternalLayout->AddWidget(pPropertyLayout);

		Widgets::Label* pLabel = new Widgets::Label(pProperty->GetName());
		pLabel->SetSizeStyle(Widgets::Widget::FIT);

		Widgets::Container* pSpacer = new Widgets::Container(10, 0);
		pSpacer->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		

		pPropertyLayout->AddWidget(pLabel);
		pPropertyLayout->AddWidget(pSpacer);
		pPropertyLayout->AddWidget(pProperty->GetEditingWidget());
	}

	void PropertyGridWidget::ClearAllItems()
	{
		for (PropertyGridItem* pProperty : m_properties)
			delete pProperty;

		m_properties.clear();

		m_pInternalLayout->DeleteAllChildren();
	}
}
