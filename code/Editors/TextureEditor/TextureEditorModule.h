/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Core/Callbacks/CallbackMacro.h"

#include "Systems/Assets/NewAssetId.h"

#include <string>

namespace Systems
{
	class AssetMetadata;
	class CubemapAsset;
	class Texture2DAsset;
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

		bool CreateAndImportTexture(const std::string& filename, Systems::Texture2DAsset** ppTexture = nullptr);
		bool DeleteTexture(Systems::NewAssetId id);
		bool RenameTexture(Systems::NewAssetId id, const std::string& newName);

		bool CreateCubemap(const std::string& assetName, Systems::CubemapAsset** ppCubemap = nullptr);

		bool ImportTexture(const Systems::NewAssetId& id);

		bool SaveTexture(const Systems::NewAssetId& id);

		bool Export(const std::string& outputFilename, const Systems::NewAssetId& id);

		EVENT_DECL(TextureCreated, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(BeforeTextureDeleted, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(AfterTextureDeleted, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(TextureRenamed, void(const Systems::AssetMetadata& pMetadata));
		EVENT_DECL(TextureSaved, void(const Systems::NewAssetId& id));
	};
}
