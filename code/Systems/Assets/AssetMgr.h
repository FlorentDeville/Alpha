/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/AssetType.h"
#include "Systems/Assets/AssetType/NewAssetType.h"
#include "Systems/Assets/Metadata/AssetMetadata.h"
#include "Systems/Assets/NewAssetId.h"

#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Systems
{
	class Asset;
	class AssetMetadata;

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
		template <typename T> void RegisterAssetType();

		const Asset* GetAsset(AssetId id) const;
		const std::vector<Asset*>& GetAssets(AssetType type) const;

		const AssetMetadata* GetMetadata(NewAssetId id) const;
		AssetMetadata* GetMetadata(NewAssetId id);

		bool DeleteMetadata(NewAssetId id);

		void ForEachMetadata(std::function<void(const Systems::AssetMetadata& metadata)> function) const;

		const std::vector<Asset*>& GetMeshes() const;
		const std::vector<Asset*>& GetMaterials() const;
		const std::vector<Asset*>& GetTextures() const;
		const std::vector<Asset*>& GetShaders() const;
		const std::vector<Asset*>& GetLevels() const;

		const NewAssetType& GetAssetType(Core::Sid sid) const;

		void GetAssets(Core::Sid assetTypeSid, Core::Array<const AssetMetadata*>& metadata) const;

		void RenameAsset(NewAssetId id, const std::string& newName);

		bool SaveTableOfContent() const;

		bool SaveMetadataTable() const;
		bool LoadMetadataTable();
		
		//Generate a new unique container id.
		ContainerId GenerateNewContainerId();

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

		std::set<ContainerId> m_containerIds;

		// New asset types. the type is saved as a string in the metadata table.
		// It is different from the asset object type. An asset object type can be upgraded and renamed,
		// but the underlying asset type is the same. So to not resave entirely the metadata table when renaming
		// a class, the asset type is separated.
		std::map<Core::Sid, NewAssetType> m_assetTypes;

		std::vector<Asset*>& Internal_GetAssets(AssetType type);

		std::string ConstructAssetPath(AssetId id, AssetType type) const;

		bool LoadTableOfContent();

		std::string GetMetadataTableFilePath() const;
	};

	template <typename T> void AssetMgr::RegisterAssetType()
	{
		const std::string& name = T::GetAssetTypeName();
		Core::Sid sid = T::GetAssetTypeNameSid();
		m_assetTypes[sid] = NewAssetType(name, sid);
	}
}
