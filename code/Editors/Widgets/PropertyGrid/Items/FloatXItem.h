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
	class FloatXItem : public PropertyGridItem
	{
	public:
		FloatXItem(void* pObj, const Core::FieldDescriptor* pField, uint32_t index, uint32_t count);
		~FloatXItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox** m_pTextbox;
		uint32_t m_count;
	};
}
