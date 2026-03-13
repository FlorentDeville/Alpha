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
	class SqtItem : public PropertyGridItem
	{
	public:
		SqtItem(Systems::Object* pObj, const Core::FieldDescriptor* pField, uint32_t index);
		~SqtItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTranslationTextbox[3];
		Widgets::TextBox* m_pRotationTextbox[3];
		Widgets::TextBox* m_pScaleTextbox[3];
	};
}
