/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Systems
{
	enum AssetType
	{
		kInvalid = 0,
		kMesh,
		kShader,
		kTexture,
		kMaterial,

		kCount 
		//Do not add anything after kCount
	};

	AssetType StringToAssetType(const std::string& type);
	
	std::string GetAssetFolder(AssetType type);

}
