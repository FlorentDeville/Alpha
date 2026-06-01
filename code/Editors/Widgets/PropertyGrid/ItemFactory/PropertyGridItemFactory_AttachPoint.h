/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/PropertyGrid/PropertyGridItemFactory.h"

#include <cstdint>

namespace Editors
{
	class PropertyGridItemFactory_AttachPoint : public PropertyGridItemFactory
	{
	public:
		PropertyGridItemFactory_AttachPoint();
		~PropertyGridItemFactory_AttachPoint();

		void CreateItems(void* pObj, const Core::FieldDescriptor* pField, uint32_t index) override;
	};
}
