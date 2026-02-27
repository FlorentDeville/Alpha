/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/TextureEditor/TextureEditorModule.h"

#include "Importer/TextureImporter/TextureImporter.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
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

	bool TextureEditorModule::CreateAndImportTexture(const std::string& filename)
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

		size_t start = filename.find_last_of('\\');
		std::string assetName = filename.substr(start + 1);
		size_t end = assetName.find_last_of('.');
		assetName = assetName.substr(0, end);

		Systems::AssetMetadata metadata(pTexture->GetId(), assetName, Systems::TextureAsset::GetAssetTypeNameSid());
		assetMgr.RegisterAssetMetadata(metadata);

		containerMgr.SaveContainer(pContainer->GetId());
		assetMgr.SaveMetadataTable();

		m_onTextureCreated(metadata);

		return true;
	}

	bool TextureEditorModule::DeleteTexture(Systems::NewAssetId id)
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
		if (!pMetadata)
			return false;

		if (!pMetadata->IsA<Systems::TextureAsset>())
			return false;

		Systems::AssetMetadata copyMetadata(*pMetadata);
		m_onBeforeTextureDeleted(copyMetadata);
		
		bool res = Systems::AssetUtil::DeleteAsset(id);
		
		m_onAfterTextureDeleted(copyMetadata);

		return res;
	}

	bool TextureEditorModule::RenameTexture(Systems::NewAssetId id, const std::string& newName)
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
		if (!pMetadata)
			return false;

		assetMgr.RenameAsset(id, newName);
		assetMgr.SaveMetadataTable();

		m_onTextureRenamed(*pMetadata);

		return true;
	}

	bool TextureEditorModule::CreateCubemap(const std::string& assetName)
	{
		Systems::CubemapAsset* pCubemap = Systems::AssetUtil::CreateAsset<Systems::CubemapAsset>(assetName);
		if (!pCubemap)
			return false;

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(pCubemap->GetId());
		if (!pMetadata)
			return false;

		m_onTextureCreated(*pMetadata);
		return true;
	}

	bool TextureEditorModule::ImportTexture(const Systems::NewAssetId& id)
	{
		if (Systems::AssetUtil::IsA<Systems::TextureAsset>(id))
		{
			if (Systems::TextureAsset* pTexture = Systems::AssetUtil::LoadAsset<Systems::TextureAsset>(id))
			{
				Importer::TextureImporter importer;
				bool res = importer.Import(pTexture->GetSourceFilename(), pTexture);
				return res;
			}

			return false;
		}
		else if (Systems::AssetUtil::IsA<Systems::CubemapAsset>(id))
		{
			if (Systems::CubemapAsset* pTexture = Systems::AssetUtil::LoadAsset<Systems::CubemapAsset>(id))
			{
				std::string filenames[6];
				filenames[0] = pTexture->GetLeftSourceFilename();
				filenames[1] = pTexture->GetRightSourceFilename();
				filenames[2] = pTexture->GetTopSourceFilename();
				filenames[3] = pTexture->GetBottomSourceFilename();
				filenames[4] = pTexture->GetFrontSourceFilename();
				filenames[5] = pTexture->GetBackSourceFilename();

				for (const std::string& filename : filenames)
				{
					if (filename.empty())
						return false;
				}

				Importer::TextureImporter importer;
				Importer::TextureImporter::Result res = importer.ImportCubemap(filenames, pTexture);
				return res.IsSuccess();
			}

			return false;
		}

		return false;
	}

	bool TextureEditorModule::SaveTexture(const Systems::NewAssetId& id)
	{
		if (!id.IsValid())
			return false;

		bool res = Systems::ContainerMgr::Get().SaveContainer(id.GetContainerId());

		if (res)
			m_onTextureSaved(id);

		return res;
	}
}