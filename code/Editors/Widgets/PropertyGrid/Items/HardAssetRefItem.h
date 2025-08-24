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
	class HardAssetRefItem : public PropertyGridItem
	{
	public:
		HardAssetRefItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);
		~HardAssetRefItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::TextBox* m_pTextbox;
	};
}
