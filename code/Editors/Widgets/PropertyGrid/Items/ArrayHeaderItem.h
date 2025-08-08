/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Editors
{
	class ArrayHeaderItem : public PropertyGridItem
	{
	public:
		ArrayHeaderItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);
		~ArrayHeaderItem();

		void CreateWidgets() override;
	};
}
