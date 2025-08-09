/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Editors
{
	class ArrayAddElementItem : public PropertyGridItem
	{
	public:
		ArrayAddElementItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField);
		~ArrayAddElementItem();

		void CreateWidgets() override;

		void OnClick_AddArrayElement();
	};
}
