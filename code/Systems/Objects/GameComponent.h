/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Objects/Object.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, GameComponent)
	class GameComponent : public Object
	{
	public:
		GameComponent();
		~GameComponent();

		virtual void Update();
		virtual void Render();

		START_REFLECTION(Systems::GameComponent)
			ADD_BASETYPE(Systems::Object)
		END_REFLECTION()
	};
}
