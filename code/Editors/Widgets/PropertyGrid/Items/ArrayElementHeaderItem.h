/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Editors
{
	class ArrayElementHeaderItem : public PropertyGridItem
	{
	public:
		ArrayElementHeaderItem(Systems::Object* pObj, const Core::FieldDescriptor* pField, uint32_t index);
		~ArrayElementHeaderItem();

		void CreateWidgets() override;
	};
}
