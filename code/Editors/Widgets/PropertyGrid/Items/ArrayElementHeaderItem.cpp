/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/ArrayElementHeaderItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"

#include "Systems/Objects/Object.h"

#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	ArrayElementHeaderItem::ArrayElementHeaderItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
	{
	}

	ArrayElementHeaderItem::~ArrayElementHeaderItem()
	{
	}

	void ArrayElementHeaderItem::CreateWidgets()
	{
		m_pEditingWidget = nullptr;
		m_pNameWidget = CreateDefaultItemLabel();
	}
}
