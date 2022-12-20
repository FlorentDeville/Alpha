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
		, m_meshes()
		, m_materials()
	{}

	AssetMgr::~AssetMgr()
	{
		for (std::pair<AssetId, Asset*> it : m_meshes)
		{
			delete it.second;
		}
		for (std::pair<AssetId, Asset*> it : m_materials)
		{
			delete it.second;
		}

		m_meshes.clear();
		m_materials.clear();
	}

	bool AssetMgr::Init(const std::string& root)
	{
		m_root = root;
		return true;
	}

	bool AssetMgr::LoadAllAssets()
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

			//convert type to AssetType
			AssetType type = StringToAssetType(strType);
			assert(type != AssetType::kInvalid);

			//get the path of the asset
			std::string subFolder = GetAssetFolder(type);
			const int PATH_MAX_LENGTH = 255;
			char path[PATH_MAX_LENGTH] = { '\0' };
			snprintf(path, PATH_MAX_LENGTH, "%s\\%s\\%08zu", m_root.c_str(), subFolder.c_str(), id);
			Asset* pNewAsset = new Asset(AssetId(id), virtualName, path, type);
			
			switch (type)
			{
			case kMesh:
				m_meshes[pNewAsset->GetId()] = pNewAsset;
				break;

			case kMaterial:
				m_materials[pNewAsset->GetId()] = pNewAsset;
				break;

			default:
				assert(false);
				break;
			}
		}

		return true;
	}

	const std::map<AssetId, Asset*>& AssetMgr::GetMeshes() const
	{
		return m_meshes;
	}
	
	const Asset* AssetMgr::GetMesh(AssetId id) const
	{
		std::map<AssetId, Asset*>::const_iterator it = m_meshes.find(id);
		if (it == m_meshes.cend())
			return nullptr;

		return it->second;
	}

	const std::map<AssetId, Asset*>& AssetMgr::GetMaterials() const
	{
		return m_materials;
	}

	const Asset* AssetMgr::GetMaterial(AssetId id) const
	{
		std::map<AssetId, Asset*>::const_iterator it = m_materials.find(id);
		if (it == m_materials.cend())
			return nullptr;

		return it->second;
	}
}
