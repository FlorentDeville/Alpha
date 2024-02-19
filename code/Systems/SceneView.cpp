/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/SceneView.h"

#include "Systems/ComponentPool.h"
#include "Systems/Scene.h"

namespace Systems
{
	SceneView::Iterator::Iterator(Scene* pScene, const ComponentMask& filter, int index)
		: m_pScene(pScene)
		, m_filter(filter)
		, m_index(index)
	{}

	EntityId SceneView::Iterator::operator*() const
	{
		return m_index;
	}

	bool SceneView::Iterator::operator==(const SceneView::Iterator& other) const
	{
		return other.m_index == m_index;
	}

	bool SceneView::Iterator::operator!=(const SceneView::Iterator& other) const
	{
		return other.m_index != m_index;
	}

	SceneView::Iterator& SceneView::Iterator::operator++()
	{
		++m_index;
		while (m_index < ComponentPool::MAX_ENTITIES)
		{
			const Entity* pEntity = m_pScene->GetConstEntity(m_index);
			if (pEntity->IsValid() && (pEntity->GetConstMask() & m_filter) == m_filter)
				return *this;

			++m_index;
		}

		return *this;
	}

	SceneView::SceneView(Scene* pScene)
		: m_pScene(pScene)
		, m_filter()
	{}

	SceneView::~SceneView()
	{}

	void SceneView::AddComponent(int componentId)
	{
		m_filter.set(componentId);
	}

	const SceneView::Iterator SceneView::begin() const
	{
		Iterator first(m_pScene, m_filter, -1);
		++first;
		return first;
	}

	const SceneView::Iterator SceneView::end() const
	{
		return Iterator(m_pScene, m_filter, ComponentPool::MAX_ENTITIES);
	}
}