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
		ArrayHeaderItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index, bool enableAddElementButton);
		~ArrayHeaderItem();

		void CreateWidgets() override;

		void OnClick_AddArrayElement();

	private:
		bool m_enableAddElementButton;
	};
}
