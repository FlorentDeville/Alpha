/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/EntityId.h"

#include <vector>

namespace Systems
{
	class ComponentPool;
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void InitComponent(int componentId, size_t componentSize);

		EntityId CreateEntity();
		void DeleteEntity(EntityId id);

		Entity* GetEntity(EntityId id);
		const Entity* GetConstEntity(EntityId id) const;

		template<class T> T* AssignComponent(EntityId id);
		template<class T> T* GetComponent(EntityId id);
		template<class T> const T* GetConstComponent(EntityId id) const;

	private:
		std::vector<Entity> m_entities;
		std::vector<ComponentPool> m_componentPools;

		std::vector<size_t> m_freeEntities;
	};
}

#include "Systems/Scene.inl"