/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Objects/Object.h"

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
		
		//Define an object id. It is persistant, saved to disk, unique, 64 bits.

		//Define an array of component.
		std::vector<GameComponent*> m_components;

		START_REFLECTION(Systems::GameObject)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD(m_name)
			ADD_FIELD(m_components)
		END_REFLECTION()
	};
}
