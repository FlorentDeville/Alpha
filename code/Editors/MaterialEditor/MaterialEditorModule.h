/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Core/Callbacks/CallbackMacro.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/NewAssetId.h"

#include <string>
#include <vector>

namespace Systems
{
	class AssetMetadata;
}

namespace Editors
{
	class MaterialEditorModule : public Core::Singleton<MaterialEditorModule>
	{
	public:
		void Init() override;

		Systems::MaterialAsset* NewMaterial(const std::string& virtualName);
		Systems::MaterialInstanceAsset* NewMaterialInstance(const std::string& virtualName, Systems::NewAssetId baseMaterialId);

		void RenameMaterial(Systems::NewAssetId id, const std::string newName);

		bool SaveMaterial(Systems::NewAssetId id);
		bool DeleteMaterial(Systems::NewAssetId id);

		const std::vector<Systems::NewAssetId>& GetAllMaterials() const;

		bool CompileMaterial(Systems::NewAssetId id);

		EVENT_DECL(MaterialCreated, void(const Systems::AssetMetadata* pMetadata));
		EVENT_DECL(MaterialInstanceCreated, void(const Systems::AssetMetadata* pMetadata));

		//This event is called for Material and MaterialInstance
		EVENT_DECL(MaterialRenamed, void(const Systems::AssetMetadata& metadata, const std::string& oldName));

	private:
		std::vector<Systems::NewAssetId> m_allMaterials;
	};
}
