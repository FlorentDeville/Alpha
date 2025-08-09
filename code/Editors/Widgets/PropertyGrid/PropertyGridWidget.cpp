/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Widgets/TableViewStyle.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	PropertyGridWidget::PropertyGridWidget()
		: Widgets::Container()
		, m_nameColumnWidth(200)
		, m_rowHeight(20)
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

	void PropertyGridWidget::AddProperty(PropertyGridItem* pProperty, int depth)
	{
		InsertProperty(pProperty, nullptr, depth);
	}

	void PropertyGridWidget::InsertProperty(PropertyGridItem* pNewProperty, const PropertyGridItem* pInsertAfter, int depth)
	{
		pNewProperty->CreateWidgets();

		Widgets::Layout* pPropertyLayout = new Widgets::Layout();
		pPropertyLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pPropertyLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::HSIZE_STRETCH | Widgets::Widget::SIZE_STYLE::VSIZE_FIT);

		if (m_pInternalLayout->GetChildrenCount() % 2 == 0)
			pPropertyLayout->GetDefaultStyle().SetBackgroundColor(Widgets::TableViewStyle::s_evenRowBackgroundColor);
		else
			pPropertyLayout->GetDefaultStyle().SetBackgroundColor(Widgets::TableViewStyle::s_oddRowBackgroundColor);

		pPropertyLayout->GetHoverStyle().SetBackgroundColor(Widgets::TableViewStyle::s_hoverBackgroundColor);

		int nameColumnWidth = m_nameColumnWidth;

		if (Widgets::Widget* pNameWidget = pNewProperty->GetNameWidget())
		{
			if (depth != 0)
			{
				const int OFFSET_DEPTH = 10;
				uint32_t containerWidth = OFFSET_DEPTH * depth;
				Widgets::Widget* pSpacer = new Widgets::Widget(containerWidth, 0, 0, 0);
				pSpacer->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
				pPropertyLayout->AddWidget(pSpacer);

				nameColumnWidth -= containerWidth;
			}

			pNameWidget->SetSizeStyle(Widgets::Widget::DEFAULT);
			pNameWidget->SetSize(Core::UInt2(nameColumnWidth, m_rowHeight));

			pPropertyLayout->AddWidget(pNameWidget);
		}
		else
		{
			Widgets::Widget* pSpacer = new Widgets::Widget(m_nameColumnWidth, 0, 0, 0);
			pSpacer->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
			pPropertyLayout->AddWidget(pSpacer);
		}

		if (pNewProperty->GetEditingWidget())
		{
			Widgets::Widget* pSpacer = new Widgets::Widget(10, 0, 0, 0);
			pSpacer->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);

			pPropertyLayout->AddWidget(pSpacer);
			pPropertyLayout->AddWidget(pNewProperty->GetEditingWidget());
		}

		if (pInsertAfter)
		{
			uint32_t position = m_properties.GetSize();

			Core::Array<PropertyGridItem*>::Iterator it = std::find(m_properties.cbegin(), m_properties.cend(), pInsertAfter);
			if (it != m_properties.cend())
				position = static_cast<uint32_t>(std::distance(m_properties.cbegin(), it) + 1);

			m_properties.Insert(pNewProperty, position);
			m_pInternalLayout->InsertWidget(pPropertyLayout, position);
		}
		else
		{
			m_properties.PushBack(pNewProperty);
			m_pInternalLayout->AddWidget(pPropertyLayout);
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void PropertyGridWidget::RemoveProperty(PropertyGridItem* pProperty)
	{
		Widgets::Widget* pPropertyWidget = pProperty->GetNameWidget()->GetParent();
		Widgets::WidgetMgr::Get().RequestWidgetDeletion(pPropertyWidget);

		m_properties.Erase(pProperty);
		delete pProperty;		
	}

	void PropertyGridWidget::ClearAllItems()
	{
		for (PropertyGridItem* pProperty : m_properties)
			delete pProperty;

		m_properties.Clear();

		m_pInternalLayout->DeleteAllChildren();
	}

	Core::Array<PropertyGridItem*>& PropertyGridWidget::GetPropertyGridItems()
	{
		return m_properties;
	}
}
