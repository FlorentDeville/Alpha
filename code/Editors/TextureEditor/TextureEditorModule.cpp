/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/TextureEditor/TextureEditorModule.h"

#include "Importer/TextureImporter/TextureImporter.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"

namespace Editors
{
	TextureEditorModule::TextureEditorModule()
		: Singleton<TextureEditorModule>()
	{ }

	TextureEditorModule::~TextureEditorModule()
	{ }

	void TextureEditorModule::Init()
	{ }

	void TextureEditorModule::Shutdown()
	{ }

	bool TextureEditorModule::Import(const std::string& filename)
	{
		Systems::TextureAsset* pTexture = Systems::CreateNewAsset<Systems::TextureAsset>();
		Importer::TextureImporter importer;
		bool res = importer.Import(filename, pTexture);
		if (!res)
		{
			Systems::DeleteObject(pTexture);
			return false;
		}

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();

		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::ContainerId cid = assetMgr.GenerateNewContainerId();
		Systems::Container* pContainer = containerMgr.CreateContainer(cid);
		pContainer->AddAsset(pTexture);

		Systems::AssetMetadata metadata(pTexture->GetId(), filename, Systems::TextureAsset::GetAssetTypeNameSid());
		assetMgr.RegisterAssetMetadata(metadata);

		containerMgr.SaveContainer(pContainer->GetId());
		assetMgr.SaveMetadataTable();

		return true;
	}
}