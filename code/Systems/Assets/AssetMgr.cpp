/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetMgr.h"

#include "Core/String/BytesToHexa.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetType.h"
#include "Systems/Assets/AssetType/NewAssetType.h"

#include <assert.h>
#include <cstdio>
#include <fstream>
#include <random>
//#pragma optimize("", off)

namespace Systems
{
	AssetMgr::AssetMgr()
		: m_root()
		, m_assets()
		, m_meshes()
		, m_materials()
		, m_nextId()
		, m_containerIds()
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

		InitAssetTypeDescription();

		LoadTableOfContent();

		return true;
	}

	const Asset* AssetMgr::CreateAsset(AssetType type, const std::string& name)
	{
		if (name.empty())
			return nullptr;

		if (name[0] != '/')
			return nullptr;

		AssetId newId(m_nextId);
		++m_nextId;

		std::string path = ConstructAssetPath(newId, type);

		Asset* pNewAsset = new Asset(newId, name, path, type);

		m_assets[newId] = pNewAsset;
		std::vector<Asset*>& assetList = Internal_GetAssets(type);
		assetList.push_back(pNewAsset);

		return pNewAsset;
	}

	bool AssetMgr::RegisterAssetMetadata(AssetMetadata& metadata)
	{
		std::map<NewAssetId, AssetMetadata>::const_iterator it = m_metadata.find(metadata.GetAssetId());
		if (it != m_metadata.cend())
			return false;

		m_metadata[metadata.GetAssetId()] = metadata;

		return true;
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

	const AssetMetadata* AssetMgr::GetMetadata(NewAssetId id) const
	{
		std::map<NewAssetId, AssetMetadata>::const_iterator it = m_metadata.find(id);
		if (it == m_metadata.cend())
			return nullptr;

		return &it->second;
	}

	AssetMetadata* AssetMgr::GetMetadata(NewAssetId id)
	{
		std::map<NewAssetId, AssetMetadata>::iterator it = m_metadata.find(id);
		if (it == m_metadata.cend())
			return nullptr;

		return &it->second;
	}

	bool AssetMgr::DeleteMetadata(NewAssetId id)
	{
		m_metadata.erase(id);
		return true;
	}

	void AssetMgr::ForEachMetadata(std::function<void(const Systems::AssetMetadata& metadata)> function) const
	{
		for (const std::pair<Systems::NewAssetId, Systems::AssetMetadata>& pair : m_metadata)
			function(pair.second);
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

	const NewAssetType& AssetMgr::GetAssetType(Core::Sid sid) const
	{
		std::map<const Core::Sid, NewAssetType>::const_iterator it = m_assetTypes.find(sid);
		if (it == m_assetTypes.cend())
		{
			assert(false && "Failed to find asset type");
			static NewAssetType ret;
			return ret;
		}

		return it->second;
	}

	void AssetMgr::GetAssets(Core::Sid assetTypeSid, Core::Array<const AssetMetadata*>& metadata) const
	{
		for(const std::pair<const NewAssetId, AssetMetadata>& pair : m_metadata)
		{
			if (pair.second.GetAssetType() == assetTypeSid)
				metadata.PushBack(&pair.second);
		}
	}

	void AssetMgr::RenameAsset(NewAssetId id, const std::string& newName)
	{
		AssetMetadata* pMetadata = GetMetadata(id);
		if (!pMetadata)
			return;

		std::string oldName = pMetadata->GetVirtualName();
		pMetadata->SetVirtualName(newName);
	}

	bool AssetMgr::SaveTableOfContent() const
	{
		std::string tocFilename = m_root + "\\toc.txt";
		std::ofstream file(tocFilename);

		for(const std::pair<AssetId, Asset*>& pair : m_assets)
		{
			const Asset* pAsset = pair.second;

			const std::string& strType = AssetTypeToString(pAsset->GetType());


			file << pAsset->GetId().ToUint64() << "," << pAsset->GetVirtualName() << "," << strType << std::endl;
		}

		file.close();

		return true;
	}

	bool AssetMgr::SaveMetadataTable() const
	{
		std::string tocFilename = GetMetadataTableFilePath();
		std::ofstream file(tocFilename);

		for (const std::pair<NewAssetId, AssetMetadata>& pair : m_metadata)
		{
			const AssetMetadata& metadata = pair.second;
			std::string aid = metadata.GetAssetId().ToString();
			const Systems::NewAssetType& type = GetAssetType(metadata.GetAssetType());
			file << aid << "," << metadata.GetVirtualName() << "," << type.GetName() << std::endl;
		}

		file.close();

		return true;
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
			if (line.empty())
				continue;

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

			std::vector<Asset*>& typedAsset = Internal_GetAssets(pNewAsset->GetType());
			typedAsset.push_back(pNewAsset);
		}

		return true;
	}

	bool AssetMgr::LoadMetadataTable()
	{
		std::string filename = GetMetadataTableFilePath();
		std::ifstream file(filename);

		const char separator = ',';
		std::string line;
		while (std::getline(file, line, separator))
		{
			if (line.empty())
				continue;

			//parse a single line
			uint64_t id = Core::HexaToUint64(line);

			std::string virtualName;
			const int VIRTUAL_NAME_MAX_LENGTH = 255;
			virtualName.reserve(VIRTUAL_NAME_MAX_LENGTH);
			std::getline(file, virtualName, separator);

			std::getline(file, line);
			Core::Sid assetTypeSid = SID(line);

			NewAssetId assetId(id);

			AssetMetadata metadata(assetId, virtualName, assetTypeSid);
			m_metadata[assetId] = metadata;

			m_containerIds.insert(assetId.GetContainerId());
		}

		return true;
	}

	ContainerId AssetMgr::GenerateNewContainerId()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

		ContainerId newId;
		do
		{
			newId = ContainerId(static_cast<uint64_t>(dis(gen)));
		} 
		while (m_containerIds.find(newId) != m_containerIds.cend());

		m_containerIds.insert(newId);
		return newId;
		
	}

	std::string AssetMgr::GetMetadataTableFilePath() const
	{
		std::string filename = m_root + "\\metadata.txt";
		return filename;
	}
}
