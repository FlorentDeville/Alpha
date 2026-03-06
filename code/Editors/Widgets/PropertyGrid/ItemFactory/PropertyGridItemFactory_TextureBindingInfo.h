/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItemFactory.h"

#include <cstdint>

namespace Systems
{
	class TextureBindingInfo;
}

namespace Widgets
{
	class TextBox;
}

namespace Editors
{
	class PropertyGridItemFactory_TextureBindingInfo : public PropertyGridItemFactory
	{
	public:
		PropertyGridItemFactory_TextureBindingInfo();
		~PropertyGridItemFactory_TextureBindingInfo();

		void CreateItems(void* pObj, const Core::FieldDescriptor* pField, uint32_t index) override;

	private:
		Widgets::TextBox* m_pTextbox;
		Systems::TextureBindingInfo* m_pBindingInfo;

		void UpdateTextbox();

		void OpenDialogBox();
	};
}
