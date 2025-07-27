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

		void SetGuid(const Core::Guid& guid);
		const Core::Guid& GetGuid() const;

		void SetName(const std::string& name);
		const std::string& GetName() const;

		TransformComponent& GetTransform();
		const TransformComponent& GetTransform() const;

	private:
		std::string m_name;
		
		Core::Guid m_guid;

		//by default all game objects have a transform
		TransformComponent m_transform;

		//Define an array of component.
		Core::Array<GameComponent*> m_components;

		START_REFLECTION(Systems::GameObject)
			ADD_BASETYPE(Systems::Object)
			ADD_FIELD_ATTR(m_guid, ReadOnly)
			ADD_FIELD(m_name)
			ADD_FIELD(m_transform)
			ADD_FIELD(m_components)
		END_REFLECTION()
	};

	GameObject* CreateNewGameObject(const TypeDescriptor* pType);

	template<typename T, typename... Args> T* CreateNewGameObject(Args... args)
	{
		T* ptr = Systems::CreateObject<T, Args...>(args...);
		ptr->SetGuid(Core::Guid::GenerateNewGuid());
		ptr->GetTransform().SetGuid(Core::Guid::GenerateNewGuid());
		return ptr;
	}

	template<typename T, typename... Args> void CreateNewGameObjectInPlace(T* ptr, Args... args)
	{
		Systems::CreateObjectInPlace<T, Args...>(ptr, args...);
		ptr->SetGuid(Core::Guid::GenerateNewGuid());
		ptr->GetTransform().SetGuid(Core::Guid::GenerateNewGuid());
	}
}
