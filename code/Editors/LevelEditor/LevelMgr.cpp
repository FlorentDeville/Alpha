/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelMgr.h"

#include "Editors/LevelEditor/SceneTree/SceneTree.h"

namespace Editors
{
	LevelMgr::LevelMgr()
		: m_name()
	{
		m_pSceneTree = new SceneTree();
	}

	LevelMgr::~LevelMgr()
	{
		delete m_pSceneTree;
	}

	const SceneTree* LevelMgr::GetConstSceneTree() const
	{
		return m_pSceneTree;
	}

	SceneTree* LevelMgr::GetSceneTree()
	{
		return m_pSceneTree;
	}

	const std::string& LevelMgr::GetName() const
	{
		return m_name;
	}
}
