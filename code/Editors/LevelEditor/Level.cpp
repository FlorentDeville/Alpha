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

	void Level::DeleteEntity(Core::TreeNode<Entity*>& node)
	{
		Entity* pEntity = node.GetContent();

		std::vector<Entity*>::const_iterator it = std::find(m_entities.cbegin(), m_entities.cend(), pEntity);
		if (it == m_entities.cend())
			return;

		m_entities.erase(it);
		node.GetParent()->DeleteChild(pEntity);

		delete pEntity;
	}


	Core::TreeNode<Entity*>& Level::GetRoot()
	{
		return *m_sceneTree.GetRoot();
	}
}
