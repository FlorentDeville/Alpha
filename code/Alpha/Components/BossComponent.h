/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/GameComponent.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"

ENABLE_GAME_REFLECTION(BossComponent)

namespace Systems
{
	class GameContext;
	class MaterialInstanceAsset;
	class MeshAsset;
}

class WaveTest;

class BossComponent : public Systems::GameComponent
{
public:
	BossComponent();
	~BossComponent();

	void PostLoad() override;

	void OnStartGame() override;
	void Update(float dt) override;
	void OnDestroyGame() override;

private:

	Systems::HardAssetRef<Systems::MeshAsset> m_mesh;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_material;

	START_REFLECTION(BossComponent)
		ADD_BASETYPE(Systems::GameComponent)
		ADD_FIELD(m_mesh)
		ADD_FIELD(m_material)
	END_REFLECTION()

	WaveTest* m_pWave;
	uint32_t m_waveIndex;
};
