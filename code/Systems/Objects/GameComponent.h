/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Guid/Guid.h"

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

	private:
		Core::Guid m_id;

		START_REFLECTION(Systems::GameComponent)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD(m_id)
		END_REFLECTION()
	};
}
