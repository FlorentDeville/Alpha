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
	class ShaderEditorModule : public Core::Singleton<ShaderEditorModule>
	{
	public:
		void Init() override;

		Systems::MaterialAsset* NewShader(const std::string& virtualName);
		bool SaveShader(Systems::NewAssetId id);
		bool DeleteShader(Systems::NewAssetId id);

		const std::vector<Systems::NewAssetId>& GetAllShaders() const;

		bool CompileShader(Systems::NewAssetId id);

		EVENT_DECL(ShaderCreated, void(Systems::AssetMetadata* pMetadata));

	private:
		std::vector<Systems::NewAssetId> m_allShaders;
	};
}
