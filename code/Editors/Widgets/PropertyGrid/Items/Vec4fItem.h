/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Widgets
{
	class TextBox;
}

namespace Editors
{
	class Vec4fItem : public PropertyGridItem
	{
	public:
		Vec4fItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);
		~Vec4fItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTextbox[4];
	};
}
