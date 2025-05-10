/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

namespace Systems
{
	class FieldDescriptor;
	class Object;
}

namespace Editors
{
	class PropertyGridWidget;

	//This class should be turned into a model
	class PropertyGridPopulator
	{
	public:
		PropertyGridPopulator();
		~PropertyGridPopulator();

		void Populate(PropertyGridWidget* pWidget, Systems::Object* pObject);

		EVENT_DECL(DataChanged, void(Systems::Object* pObject, const Systems::FieldDescriptor* pField))
	};
}
