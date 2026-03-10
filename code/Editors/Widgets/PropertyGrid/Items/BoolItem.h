/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Widgets
{
	class CheckBox;
}

namespace Editors
{
	class BoolItem : public PropertyGridItem
	{
	public:
		BoolItem(Systems::Object* pObj, const Core::FieldDescriptor* pField, uint32_t index);
		~BoolItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::CheckBox* m_pCheckbox;
	};
}
