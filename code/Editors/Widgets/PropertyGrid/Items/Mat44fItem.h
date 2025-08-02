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
	class Mat44fItem : public PropertyGridItem
	{
	public:
		Mat44fItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);
		~Mat44fItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTextbox[4][4];
	};
}
