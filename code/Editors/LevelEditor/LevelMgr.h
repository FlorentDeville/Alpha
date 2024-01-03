/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Editors
{
	class SceneTree;

	class LevelMgr
	{
	public:
		LevelMgr();
		~LevelMgr();

		const SceneTree* GetConstSceneTree() const;
		SceneTree* GetSceneTree();

		const std::string& GetName() const;

	private:
		std::string m_name;
		SceneTree* m_pSceneTree;
	};
}
