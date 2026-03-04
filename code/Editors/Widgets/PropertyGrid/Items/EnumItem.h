/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Widgets
{
	class ComboBox;
}

namespace Editors
{
	class EnumItem : public PropertyGridItem
	{
	public:
		EnumItem(void* pObj, const Core::FieldDescriptor* pField, uint32_t index);
		~EnumItem();

		void CreateWidgets() override;
		void UpdateValue() override;

		void OnSelectionChanged(int64_t oldValue, int64_t newValue);

	private:
		Widgets::ComboBox* m_pComboBox;

		int64_t m_selectedEnum;
	};
}
