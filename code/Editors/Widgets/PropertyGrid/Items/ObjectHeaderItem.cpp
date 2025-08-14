/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ObjectHeaderItem.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Editors
{
	ObjectHeaderItem::ObjectHeaderItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
	{
	}

	ObjectHeaderItem::~ObjectHeaderItem()
	{
	}

	void ObjectHeaderItem::CreateWidgets()
	{
		
		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };

		sprintf_s(buffer, "%s (%s)", m_pField->GetName().c_str(), m_pField->GetType()->GetName().c_str());

		Widgets::Label* pNameLabel = new Widgets::Label(buffer);
		pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);

		Widgets::Layout* pNameLayout = new Widgets::Layout(Widgets::Layout::Horizontal, Widgets::Widget::FIT);
		pNameLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->SetSpace(DirectX::XMINT2(5, 0));
		pNameLayout->AddWidget(pNameLabel);

		m_pNameWidget = pNameLayout;
	}
}
