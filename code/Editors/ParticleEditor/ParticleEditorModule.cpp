/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/ParticleEditor/ParticleEditorModule.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"

namespace Editors
{
	ParticleEditorModule::ParticleEditorModule()
		: Core::Singleton<ParticleEditorModule>()
	{ }

	ParticleEditorModule::~ParticleEditorModule()
	{ }

	void ParticleEditorModule::Init()
	{ }

	void ParticleEditorModule::Shutdown()
	{ }

	Systems::ParticleEffectAsset* ParticleEditorModule::CreateParticleEffect(const std::string& name)
	{
		Systems::ParticleEffectAsset* pAsset = Systems::AssetUtil::CreateAsset<Systems::ParticleEffectAsset>(name);
		if (!pAsset)
			return nullptr;

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(pAsset->GetId());
		assert(pMetadata);

		m_onParticleEffectCreated(*pMetadata);

		return pAsset;
	}

	void ParticleEditorModule::DeleteParticleEffect(Systems::NewAssetId id)
	{
		m_onBeforeParticleEffectDeleted(id);

		bool res = Systems::AssetUtil::DeleteAsset(id);
		assert(res);

		if (!res)
			return;

		m_onAfterParticleEffectDeleted(id);
	}

	void ParticleEditorModule::RenameParticleEffect(Systems::NewAssetId id, const std::string& newName)
	{
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
			return;

		pMetadata->SetVirtualName(newName);
		Systems::AssetMgr::Get().SaveMetadataTable();

		m_onParticleEffectRenamed(*pMetadata);
	}

	void ParticleEditorModule::SaveParticleEffect(Systems::NewAssetId id)
	{
		bool res = Systems::ContainerMgr::Get().SaveContainer(id.GetContainerId());
		if (!res)
			return;

		m_onParticleEffectSaved(id);
	}
}
