/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Scene.h"

#include "Systems/ComponentPool.h"
#include "Systems/Entity.h"

namespace Systems
{
	Scene::Scene()
	{
		m_componentPools.resize(MAX_COMPONENTS);
		m_freeEntities.reserve(ComponentPool::MAX_ENTITIES);
		m_entities.reserve(ComponentPool::MAX_ENTITIES);
	}

	Scene::~Scene()
	{}

	void Scene::InitComponent(int componentId, size_t componentSize)
	{
		m_componentPools[componentId].Init(componentSize);
	}

	EntityId Scene::CreateEntity()
	{
		EntityId id = INVALID_ENTITY_ID;
		if (!m_freeEntities.empty())
		{
			id = m_freeEntities.back();
			m_freeEntities.pop_back();
		}
		else
		{
			id = m_entities.size();
		}

		m_entities.push_back(Systems::Entity(id));
		return id;
	}

	void Scene::DeleteEntity(EntityId id)
	{
		Entity& entity = m_entities[id];
		if (!entity.IsValid())
			return;

		entity.MakeInvalid();

		m_freeEntities.push_back(id);
	}

	Entity* Scene::GetEntity(EntityId id)
	{
		Entity& entity = m_entities[id];
		return &entity;
	}

	const Entity* Scene::GetConstEntity(EntityId id) const
	{
		const Entity& entity = m_entities[id];
		return &entity;
	}
}
