/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Core/Guid/Guid.h"

#include "Systems/Objects/Object.h"
#include "Systems/GameComponent/TransformComponent.h"
#include "Systems/Reflection/ReflectionStandardTypes.h"

namespace Systems
{
	class GameComponent;

	ENABLE_REFLECTION_WITH_NS(Systems, GameObject)
	class GameObject : public Object
	{
	public:
		GameObject() = default;
		GameObject(const std::string& name);

		~GameObject() = default;

		virtual void Update();
		virtual void Render();

	private:
		std::string m_name;
		
		Core::Guid m_id;

		//by default all game objects have a transform
		TransformComponent m_transform;

		//Define an array of component.
		Core::Array<GameComponent*> m_components;

		START_REFLECTION(Systems::GameObject)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD(m_id)
			ADD_FIELD(m_transform)
			ADD_FIELD(m_name)
			ADD_FIELD(m_components)
		END_REFLECTION()
	};
}
