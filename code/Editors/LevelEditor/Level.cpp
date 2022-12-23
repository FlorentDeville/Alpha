/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Level.h"

#include "Editors/LevelEditor/Entity.h"

namespace Editors
{
	Level::Level()
		: m_entities()
		, m_sceneTree()
	{}

	Level::~Level()
	{
		for (Entity* pEntity : m_entities)
			delete pEntity;

		m_entities.clear();
		m_sceneTree.clear();
	}

	Core::TreeNode<Entity*>& Level::AddEntity(Entity* pNewEntity, Core::TreeNode<Entity*>& parent)
	{
		m_entities.push_back(pNewEntity);
		Core::TreeNode<Entity*>* pNewNode = parent.AddChildren(pNewEntity);
		return *pNewNode;
	}

	Core::TreeNode<Entity*>& Level::GetRoot()
	{
		return *m_sceneTree.GetRoot();
	}
}
