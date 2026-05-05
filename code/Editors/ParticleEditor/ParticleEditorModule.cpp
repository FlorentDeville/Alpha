/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/ParticleEditor/ParticleEditorModule.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"
#include "Systems/Game/Subsystems/Clock/GameClockSubsystem.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/Subsystems/Particle/ParticleSystem.h"

namespace Editors
{
	ParticleEditorModule::ParticleEditorModule()
		: Core::Singleton<ParticleEditorModule>()
		, m_pGameContext(nullptr)
	{ }

	ParticleEditorModule::~ParticleEditorModule()
	{ }

	void ParticleEditorModule::Init()
	{
		m_pGameContext = new Systems::GameContext();

		m_pGameContext->m_pParticleSystem = new Systems::ParticleSystem();
		m_pGameContext->m_pClock = new Systems::GameClockSubsystem();
		m_pGameContext->m_pClock->Start();
	}

	void ParticleEditorModule::Shutdown()
	{
		delete m_pGameContext;
	}

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

	const Systems::GameContext* ParticleEditorModule::GetGameContext() const
	{
		return m_pGameContext;
	}

	Systems::GameContext* ParticleEditorModule::GetGameContext()
	{
		return m_pGameContext;
	}
}
