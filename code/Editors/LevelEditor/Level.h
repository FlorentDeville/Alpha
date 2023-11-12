/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

//#include "Core/Tree/Tree.h"

#include <string>

namespace Editors
{
	class Entity;
	class SceneTree;

	class Level
	{
	public:
		Level();
		~Level();

		//Core::TreeNode<Entity*>& AddEntity(Entity* pNewEntity, Core::TreeNode<Entity*>& parent);

		//void DeleteEntity(Core::TreeNode<Entity*>& node);

		//Core::TreeNode<Entity*>& GetRoot();

		const SceneTree* GetConstSceneTree() const;
		SceneTree* GetSceneTree();

	private:
		std::string m_name;
		SceneTree* m_pSceneTree;
		//std::vector<Entity*> m_entities; //list of entities. This list owns them.
		//Core::Tree<Entity*> m_sceneTree; //tree of entities.
	};
}
