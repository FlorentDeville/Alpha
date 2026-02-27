/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Core/Callbacks/CallbackMacro.h"

#include "Systems/Assets/NewAssetId.h"

#include <string>

namespace Systems
{
	class AssetMetadata;
}

namespace Editors
{
	class TextureEditorModule : public Core::Singleton<TextureEditorModule>
	{
	public:
		TextureEditorModule();
		~TextureEditorModule();

		void Init() override;
		void Shutdown() override;

		bool ImportTexture(const std::string& filename);
		bool DeleteTexture(Systems::NewAssetId id);
		bool RenameTexture(Systems::NewAssetId id, const std::string& newName);

		bool CreateNewCubemap(const std::string& assetName);

		EVENT_DECL(TextureCreated, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(BeforeTextureDeleted, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(AfterTextureDeleted, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(TextureRenamed, void(const Systems::AssetMetadata& pMetadata));
	};
}
