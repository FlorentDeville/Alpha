/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Systems
{
	class GameObject;
}

namespace Widgets
{
	class TextBox;
}

namespace Editors
{
	class ComponentRefItem : public PropertyGridItem
	{
	public:
		ComponentRefItem(void* pObj, const Core::FieldDescriptor* pField, uint32_t index, const Systems::GameObject* pOwner);
		~ComponentRefItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTextbox;
		const Systems::GameObject* m_pOwner;
	};
}
