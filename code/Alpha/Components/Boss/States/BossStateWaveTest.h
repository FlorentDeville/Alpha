/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

namespace Systems
{
	class MeshAsset;
	class MaterialInstanceAsset;
}

class BossComponent;
class WaveTest;

class BossStateWaveTest : public IState
{
public:
	BossStateWaveTest(StateMachine* pStateMachine);
	~BossStateWaveTest();

	void Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, BossComponent* pComponent);

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	WaveTest* m_pWave;
	BossComponent* m_pBossComponent;
	uint32_t m_waveIndex;
};
