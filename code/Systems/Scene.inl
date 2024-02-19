/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/ComponentId.h"
#include "Systems/Entity.h"

#include <assert.h>

namespace Systems
{
	template<class T> T* Scene::AssignComponent(EntityId id)
	{
		int componentId = GetComponentId<T>();
		assert(componentId < MAX_COMPONENTS); //too many components

		m_entities[id].GetMask().set(componentId);

		void* pData = m_componentPools[componentId].Get(id);
		T* pComponent = new (pData) T();
		return pComponent;
	}

	template<class T> T* Scene::GetComponent(EntityId id)
	{
		if (!m_entities[id].IsValid())
			return nullptr;

		int componentId = GetComponentId<T>();
		return m_componentPools[componentId].Get(id);
	}

	template<class T> const T* Scene::GetConstComponent(EntityId id) const
	{
		if (!m_entities[id].IsValid())
			return nullptr;

		int componentId = GetComponentId<T>();
		return m_componentPools[componentId].Get(id);
	}
}