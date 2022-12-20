/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetType.h"

namespace Systems
{
	AssetType StringToAssetType(const std::string& type)
	{
		if (type == "MESH")
			return kMesh;
		else if (type == "MATERIAL")
			return kMaterial;
		else if (type == "TEXTURE")
			return kTexture;
		else
			return kInvalid;
	}

	std::string GetAssetFolder(AssetType type)
	{
		switch (type)
		{
		case kMesh:
			return "mesh";
			break;

		case kMaterial:
			return "materials";
			break;

		case kTexture:
			return "textures";
			break;

		default:
			return "DEADMEAT";
			break;
		}
	}
}
