/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Editors
{
	class ObjectHeaderItem : public PropertyGridItem
	{
	public:
		ObjectHeaderItem(Systems::Object* pObj, const Core::FieldDescriptor* pField, uint32_t index);
		~ObjectHeaderItem();

		void CreateWidgets() override;
	};
}
