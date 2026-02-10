/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Widgets
{
	class TextBox;
}

namespace Editors
{
	class FloatItem : public PropertyGridItem
	{
	public:
		FloatItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);
		~FloatItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTextbox;
	};
}
