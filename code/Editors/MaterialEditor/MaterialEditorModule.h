/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Core/Callbacks/CallbackMacro.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
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
		bool SaveMaterial(Systems::NewAssetId id);
		bool DeleteMaterial(Systems::NewAssetId id);

		const std::vector<Systems::NewAssetId>& GetAllShaders() const;

		bool CompileMaterial(Systems::NewAssetId id);

		EVENT_DECL(ShaderCreated, void(Systems::AssetMetadata* pMetadata));

	private:
		std::vector<Systems::NewAssetId> m_allShaders;
	};
}
