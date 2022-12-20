/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Systems/Assets/AssetId.h"

#include <map>
#include <string>

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

		const std::map<AssetId, Asset*>& GetMeshes() const;
		const Asset* GetMesh(AssetId id) const;
		
		const std::map<AssetId, Asset*>& GetMaterials() const;
		const Asset* GetMaterial(AssetId id) const;

	private:
		std::string m_root; //location of toc.txt

		std::map<AssetId, Asset*> m_meshes;
		std::map<AssetId, Asset*> m_materials;
	};

}
