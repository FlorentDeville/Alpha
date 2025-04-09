/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetType.h"

#include <cassert>
#include <map>
#include <vector>

namespace Systems
{
	struct AssetTypeDescription
	{
		AssetType m_type;
		std::string m_strType;
		std::string m_folder;

		AssetTypeDescription()
			: m_type(kInvalid)
			, m_strType()
			, m_folder()
		{}

		AssetTypeDescription(AssetType type, const std::string& strType, const std::string& folder)
			: m_type(type)
			, m_strType(strType)
			, m_folder(folder)
		{}
	};

	std::vector<AssetTypeDescription> s_assetTypeDescription;
	std::map<std::string, AssetType> s_stringToAssetType;

	bool InitAssetTypeDescription()
	{
		s_assetTypeDescription.resize(static_cast<int>(kCount));
		s_assetTypeDescription[static_cast<int>(kMesh)] = AssetTypeDescription(kMesh, "MESH", "mesh");
		s_assetTypeDescription[static_cast<int>(kMaterial)] = AssetTypeDescription(kMaterial, "MATERIAL", "materials");
		s_assetTypeDescription[static_cast<int>(kTexture)] = AssetTypeDescription(kTexture, "TEXTURE", "textures");
		s_assetTypeDescription[static_cast<int>(kShader)] = AssetTypeDescription(kShader, "SHADER", "shaders");
		s_assetTypeDescription[static_cast<int>(kLevel)] = AssetTypeDescription(kLevel, "LEVEL", "levels");

		s_stringToAssetType["MESH"] = kMesh;
		s_stringToAssetType["MATERIAL"] = kMaterial;
		s_stringToAssetType["TEXTURE"] = kTexture;
		s_stringToAssetType["SHADER"] = kShader;
		s_stringToAssetType["LEVEL"] = kLevel;

		return true;
	}

	AssetType StringToAssetType(const std::string& type)
	{
		std::map<std::string, AssetType>::const_iterator it = s_stringToAssetType.find(type);
		if (it == s_stringToAssetType.cend())
			return kInvalid;

		return it->second;
	}

	const std::string& AssetTypeToString(AssetType type)
	{
		assert(type != kInvalid);
		return s_assetTypeDescription[type].m_strType;
	}

	const std::string& GetAssetFolder(AssetType type)
	{
		assert(type != kInvalid);
		return s_assetTypeDescription[type].m_folder;
	}
}
