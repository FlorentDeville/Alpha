/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Systems
{
	enum AssetType
	{
		kInvalid = -1,
		kMesh,
		kShader,
		kTexture,
		kMaterial,
		kLevel,

		kCount 
		//Do not add anything after kCount
	};

	bool InitAssetTypeDescription();

	AssetType StringToAssetType(const std::string& type);
	const std::string& AssetTypeToString(AssetType type);
	
	const std::string& GetAssetFolder(AssetType type);

}
