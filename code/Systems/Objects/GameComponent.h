/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Guid/Guid.h"

#include "Systems/Objects/Object.h"
#include "Core/Reflection/ReflectionMacro.h"

ENABLE_REFLECTION(Systems, GameComponent)

namespace Systems
{
	class GameObject;
	class GameContext;

	class GameComponent : public Object
	{
	public:
		GameComponent();
		~GameComponent();

		//Called once in game and editor before the first update
		virtual void OnStart(GameContext* pWorld);

		//Called once in game only before the first update
		virtual void OnStartGame();

		virtual void Update(float dt);
		
		virtual void OnDestroy(GameContext* pWorld);
		virtual void OnDestroyGame();

		const Core::Guid& GetGuid() const;
		void SetGuid(const Core::Guid& guid);

		void SetOwner(GameObject* pOwnerGo);

		const GameObject* GetOwner() const;
		GameObject* GetOwner();

	private:
		Core::Guid m_guid;

		START_REFLECTION(Systems::GameComponent)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD_ATTR(m_guid, Core::Hidden)
		END_REFLECTION()

		GameObject* m_pOwnerGo;
	};

	GameComponent* CreateNewGameComponent(const Core::TypeDescriptor* pType);

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
