/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItemFactory.h"

#include <cstdint>

namespace Editors
{
	class PropertyGridItemFactory_Transform : public PropertyGridItemFactory
	{
	public:
		PropertyGridItemFactory_Transform();
		~PropertyGridItemFactory_Transform();

		void CreateItems(void* pObj, const Core::FieldDescriptor* pField, uint32_t index) override;
	};
}
