/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Systems/Assets/AssetId.h"

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

		bool LoadAllAssets();

		const Asset* GetAsset(AssetId id) const;

		const std::vector<Asset*>& GetMeshes() const;
		const std::vector<Asset*>& GetMaterials() const;

	private:
		std::string m_root; //location of toc.txt

		std::map<AssetId, Asset*> m_assets; //use this structure to retrieve an asset from its id

		//use those vectors to iterate through a given type of asset.
		std::vector<Asset*> m_meshes;
		std::vector<Asset*> m_materials;
	};

}
