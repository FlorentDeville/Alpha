/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"

namespace Widgets
{
	class Container;
	class SliderFloat;
}

namespace Editors
{
	class ColorItem : public PropertyGridItem
	{
	public:
		ColorItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index);
		~ColorItem();

		void CreateWidgets() override;
		void UpdateValue() override;

	private:
		Widgets::SliderFloat* m_pSliderRedChannel;
		Widgets::SliderFloat* m_pSliderGreenChannel;
		Widgets::SliderFloat* m_pSliderBlueChannel;
		Widgets::Container* m_pColorWidget;
	};
}
