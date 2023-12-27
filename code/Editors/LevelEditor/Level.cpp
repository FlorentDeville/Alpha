/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Level.h"

#include "Editors/LevelEditor/SceneTree/SceneTree.h"

namespace Editors
{
	Level::Level()
		: m_name()
	{
		m_pSceneTree = new SceneTree();
	}

	Level::~Level()
	{
		delete m_pSceneTree;
	}

	const SceneTree* Level::GetConstSceneTree() const
	{
		return m_pSceneTree;
	}

	SceneTree* Level::GetSceneTree()
	{
		return m_pSceneTree;
	}
}
