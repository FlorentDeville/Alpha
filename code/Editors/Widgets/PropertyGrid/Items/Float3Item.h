/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Widgets
{
	class TextBox;
}

namespace Editors
{
	class Float3Item : public PropertyGridItem
	{
	public:
		Float3Item(void* pObj, const Core::FieldDescriptor* pField, uint32_t index);
		~Float3Item();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTextbox[3];
	};
}
