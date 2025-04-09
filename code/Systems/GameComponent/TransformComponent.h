/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Systems/Objects/GameComponent.h"
#include "Systems/Reflection/ReflectionCoreTypes.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, TransformComponent)
	class TransformComponent : public GameComponent
	{
	public:
		TransformComponent();
		~TransformComponent();

	private:
		Core::Mat44f m_localTransform;

		START_REFLECTION(Systems::TransformComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_localTransform)
		END_REFLECTION()
	};
}

