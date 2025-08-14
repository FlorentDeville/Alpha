/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItemFactory.h"

namespace Widgets
{
	class TextBox;
}

namespace Editors
{
	class PropertyGridItemFactory_MaterialParameterDescription : public PropertyGridItemFactory
	{
	public:
		PropertyGridItemFactory_MaterialParameterDescription();
		~PropertyGridItemFactory_MaterialParameterDescription();

		void CreateItems(void* pObj, const Systems::FieldDescriptor* pField, int index) override;

	private:
		Widgets::TextBox* CreateSingleFloatWidget(float* pData);
	};
}
