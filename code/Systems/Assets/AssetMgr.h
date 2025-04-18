/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/AssetType.h"
#include "Systems/Assets/Metadata/AssetMetadata.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Reflection/ReflectionMgr.h"
#include "Systems/Reflection/TypeDescriptor.h"

#include <map>
#include <string>
#include <vector>

namespace Systems
{
	class Asset;

	class AssetMgr : public Core::Singleton<AssetMgr>
	{
		friend Core::Singleton<AssetMgr>;

	protected:
		AssetMgr();
		~AssetMgr();

	public:
		bool Init(const std::string& root);

		const Asset* CreateAsset(AssetType type, const std::string& name);

		bool RegisterAssetMetadata(AssetMetadata& metadata);

		const Asset* GetAsset(AssetId id) const;
		const std::vector<Asset*>& GetAssets(AssetType type) const;

		const std::vector<Asset*>& GetMeshes() const;
		const std::vector<Asset*>& GetMaterials() const;
		const std::vector<Asset*>& GetTextures() const;
		const std::vector<Asset*>& GetShaders() const;
		const std::vector<Asset*>& GetLevels() const;

		template<typename T> void GetAssets(std::vector<AssetMetadata*>& metadata);

		bool SaveTableOfContent() const;

		bool SaveMetadataTable() const;

	private:
		std::string m_root; //location of toc.txt
		
		size_t m_nextId; //next available asset id

		std::map<AssetId, Asset*> m_assets; //use this structure to retrieve an asset from its id

		//use those vectors to iterate through a given type of asset.
		std::vector<Asset*> m_meshes;
		std::vector<Asset*> m_materials;
		std::vector<Asset*> m_textures;
		std::vector<Asset*> m_shaders;
		std::vector<Asset*> m_levels;

		// New asset system using containers
		std::map<NewAssetId, AssetMetadata> m_metadata;

		std::vector<Asset*>& Internal_GetAssets(AssetType type);

		std::string ConstructAssetPath(AssetId id, AssetType type) const;

		bool LoadTableOfContent();
		bool LoadMetadataTable();

		std::string GetMetadataTableFilePath() const;
	};

	template<typename T> void AssetMgr::GetAssets(std::vector<AssetMetadata*>& metadata)
	{
		Core::Sid sid = TypeResolver<T>::GetConstType()->GetSid();

		for (std::pair<const NewAssetId, AssetMetadata>& pair : m_metadata)
		{
			if (pair.second.GetClassSid() == sid)
				metadata.push_back(&pair.second);
		}
	}
}
