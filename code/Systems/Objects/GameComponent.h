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

		void SetGuid(const Core::Guid& guid);

	private:
		Core::Guid m_guid;

		START_REFLECTION(Systems::GameComponent)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD(m_guid)
		END_REFLECTION()
	};

	template<typename T, typename... Args> T* CreateNewGameComponent(Args... args)
	{
		T* ptr = Systems::CreateObject(std::forward<Args>(args)...);
		ptr->SetGuid(Core::Guid::GenerateNewGuid());
		return ptr;
	}

	template<typename T, typename... Args> void CreateNewGameComponentInPlace(T* ptr, Args... args)
	{
		Systems::CreateObjectInPlace(ptr, std::forward<Args>(args)...);
		ptr->SetGuid(Core::Guid::GenerateNewGuid());
	}
}
