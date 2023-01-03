/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Tree/Tree.h"

#include <string>

namespace Editors
{
	class Entity;

	class Level
	{
	public:
		Level();
		~Level();

		Core::TreeNode<Entity*>& AddEntity(Entity* pNewEntity, Core::TreeNode<Entity*>& parent);

		void DeleteEntity(Core::TreeNode<Entity*>& node);

		Core::TreeNode<Entity*>& GetRoot();

	private:
		std::string m_name;
		std::vector<Entity*> m_entities; //list of entities. This list owns them.
		Core::Tree<Entity*> m_sceneTree; //tree of entities.
	};
}
