/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ArrayHeaderItem.h"

namespace Editors
{
	ArrayHeaderItem::ArrayHeaderItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
	{
	}

	ArrayHeaderItem::~ArrayHeaderItem()
	{
	}

	void ArrayHeaderItem::CreateWidgets()
	{
		m_pNameWidget = CreatePodItemLabel();
		m_pEditingWidget = nullptr;
	}
}
