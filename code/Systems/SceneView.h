/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/EntityId.h"
#include "Systems/Entity.h"

namespace Systems
{
	class Scene;

	class SceneView
	{
	public:
		class Iterator
		{
		public:
			Iterator(Scene* pScene, const ComponentMask& filter, int index);

			EntityId operator*() const;
			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;
			Iterator& operator++();

		private:
			Scene* m_pScene;
			ComponentMask m_filter;
			int m_index;
		};

		SceneView(Scene* pScene);
		~SceneView();

		void AddComponent(int componentId);

		const Iterator begin() const;
		const Iterator end() const;

	private:
		Scene* m_pScene;
		ComponentMask m_filter; //component necessary for an entity to be part of the view
	};
}
