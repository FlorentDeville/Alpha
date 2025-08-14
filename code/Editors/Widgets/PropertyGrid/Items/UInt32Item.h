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
	class UInt32Item : public PropertyGridItem
	{
	public:
		UInt32Item(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);
		~UInt32Item();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTextbox;
	};
}
