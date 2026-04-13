/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"

#include <string>

namespace Systems
{
	class AssetMetadata;
	class ParticleEffectAsset;
}

namespace Editors
{
	class ParticleEditorModule : public Core::Singleton<ParticleEditorModule>
	{
	public:
		ParticleEditorModule();
		~ParticleEditorModule();

		void Init() override;
		void Shutdown() override;

		Systems::ParticleEffectAsset* CreateParticleEffect(const std::string& name);
		void DeleteParticleEffect(Systems::NewAssetId id);
		void RenameParticleEffect(Systems::NewAssetId id, const std::string& newName);

		EVENT_DECL(ParticleEffectCreated, void(const Systems::AssetMetadata& metadata))
		EVENT_DECL(BeforeParticleEffectDeleted, void(Systems::NewAssetId))
		EVENT_DECL(AfterParticleEffectDeleted, void(Systems::NewAssetId))
		EVENT_DECL(ParticleEffectRenamed, void(Systems::AssetMetadata& metadata))
	};
}
