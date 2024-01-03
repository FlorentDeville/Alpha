/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Systems
{
	class Asset;
}

namespace Editors
{
	class SceneTree;

	class LevelSerializer
	{
	public:
		static bool Serialize(const Systems::Asset& asset, const std::string& levelName, const SceneTree* pSceneTree);
		static bool Deserialize(const Systems::Asset& asset, std::string& levelName, SceneTree* pSceneTree);
	};
}
