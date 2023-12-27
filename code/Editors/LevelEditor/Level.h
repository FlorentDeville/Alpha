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

		const SceneTree* GetConstSceneTree() const;
		SceneTree* GetSceneTree();

	private:
		std::string m_name;
		SceneTree* m_pSceneTree;
	};
}
