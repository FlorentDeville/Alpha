/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetMgr.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetType.h"

#include <assert.h>
#include <fstream>

//#pragma optimize("", off)

namespace Systems
{
	AssetMgr::AssetMgr()
		: m_root()
		, m_assets()
		, m_meshes()
		, m_materials()
		, m_nextId()
	{}

	AssetMgr::~AssetMgr()
	{
		for (std::pair<AssetId, Asset*> it : m_assets)
		{
			delete it.second;
		}

		m_assets.clear();
		m_meshes.clear();
		m_materials.clear();
	}

	bool AssetMgr::Init(const std::string& root)
	{
		m_root = root;

		LoadTableOfContent();

		return true;
	}

	const Asset* AssetMgr::CreateAsset(AssetType type, const std::string& name)
	{
		AssetId newId(m_nextId);
		++m_nextId;

		std::string path = ConstructAssetPath(newId, type);

		Asset* pNewAsset = new Asset(newId, name, path, type);

		m_assets[newId] = pNewAsset;
		std::vector<Asset*>& assetList = Internal_GetAssets(type);
		assetList.push_back(pNewAsset);

		return pNewAsset;
	}

	const Asset* AssetMgr::GetAsset(AssetId id) const
	{
		std::map<AssetId, Asset*>::const_iterator it = m_assets.find(id);
		if (it == m_assets.cend())
			return nullptr;

		return it->second;
	}

	const std::vector<Asset*>& AssetMgr::GetAssets(AssetType type) const
	{
		switch (type)
		{
		case kMesh:
			return m_meshes;
			break;

		case kMaterial:
			return m_materials;
			break;

		case kTexture:
			return m_textures;
			break;

		case kShader:
			return m_shaders;
			break;

		case kLevel:
			return m_levels;
			break;

		default:
			assert(false);
			break;
		}

		//we should never reach this
		assert(false);
		return m_meshes;
	}

	const std::vector<Asset*>& AssetMgr::GetMeshes() const
	{
		return m_meshes;
	}
	
	const std::vector<Asset*>& AssetMgr::GetMaterials() const
	{
		return m_materials;
	}

	const std::vector<Asset*>& AssetMgr::GetTextures() const
	{
		return m_textures;
	}

	const std::vector<Asset*>& AssetMgr::GetShaders() const
	{
		return m_shaders;
	}

	const std::vector<Asset*>& AssetMgr::GetLevels() const
	{
		return m_levels;
	}

	std::vector<Asset*>& AssetMgr::Internal_GetAssets(AssetType type)
	{
		switch (type)
		{
		case kMesh:
			return m_meshes;
			break;

		case kMaterial:
			return m_materials;
			break;

		case kTexture:
			return m_textures;
			break;

		case kShader:
			return m_shaders;
			break;

		case kLevel:
			return m_levels;
			break;

		default:
			assert(false);
			break;
		}

		//we should never reach this
		assert(false);
		return m_meshes;
	}

	std::string AssetMgr::ConstructAssetPath(AssetId id, AssetType type) const
	{
		std::string subFolder = GetAssetFolder(type);
		return m_root + "\\" + subFolder + "\\" + id.ToString();
	}

	bool AssetMgr::LoadTableOfContent()
	{
		std::string tocFilename = m_root + "\\toc.txt";
		std::ifstream file(tocFilename);

		std::string line;
		while (std::getline(file, line))
		{
			//parse a single line
			size_t id;
			const int VIRTUAL_NAME_MAX_LENGTH = 255;
			char virtualName[VIRTUAL_NAME_MAX_LENGTH] = { '\0' };
			const int TYPE_MAX_LENGTH = 16;
			char strType[TYPE_MAX_LENGTH] = { '\0' };
			sscanf_s(line.c_str(), "%zu,%[^,],%s", &id, virtualName, VIRTUAL_NAME_MAX_LENGTH, strType, TYPE_MAX_LENGTH);
			AssetId assetId(id);

			//convert type to AssetType
			AssetType type = StringToAssetType(strType);
			assert(type != AssetType::kInvalid);

			//get the path of the asset
			std::string path = ConstructAssetPath(assetId, type);

			if (id >= m_nextId)
				m_nextId = id + 1;

			Asset* pNewAsset = new Asset(assetId, virtualName, path, type);

			m_assets[pNewAsset->GetId()] = pNewAsset;

			switch (type)
			{
			case kMesh:
				m_meshes.push_back(pNewAsset);
				break;

			case kMaterial:
				m_materials.push_back(pNewAsset);
				break;

			case kTexture:
				m_textures.push_back(pNewAsset);
				break;

			case kShader:
				m_shaders.push_back(pNewAsset);
				break;

			case kLevel:
				m_levels.push_back(pNewAsset);
				break;

			default:
				assert(false);
				break;
			}
		}

		return true;
	}
}
