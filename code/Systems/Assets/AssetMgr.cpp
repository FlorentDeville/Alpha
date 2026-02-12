/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetMgr.h"

#include "Core/String/BytesToHexa.h"

#include <assert.h>
#include <fstream>
#include <random>
//#pragma optimize("", off)

namespace Systems
{
	AssetMgr::AssetMgr()
		: m_root()
		, m_nextId()
		, m_containerIds()
	{}

	AssetMgr::~AssetMgr()
	{ }

	bool AssetMgr::Init(const std::string& root)
	{
		m_root = root;
		return true;
	}

	bool AssetMgr::RegisterAssetMetadata(AssetMetadata& metadata)
	{
		std::map<NewAssetId, AssetMetadata>::const_iterator it = m_metadata.find(metadata.GetAssetId());
		if (it != m_metadata.cend())
			return false;

		m_metadata[metadata.GetAssetId()] = metadata;

		return true;
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

	const NewAssetType* AssetMgr::GetAssetTypeFromClassName(Core::Sid className) const
	{
		for (const std::pair<const Core::Sid, NewAssetType>& element : m_assetTypes)
		{
			const NewAssetType& assetType = element.second;
			if (assetType.GetClassName() == className)
				return &assetType;
		}

		return nullptr;
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
