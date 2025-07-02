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

		void CreateItems(const Systems::TypeDescriptor* pType, void* pData, int depth) override;

	private:
		Widgets::TextBox* CreateSingleFloatWidget(float* pData);
	};
}
