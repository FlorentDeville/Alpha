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

		default:
			return "DEADMEAT";
			break;
		}
	}
}
