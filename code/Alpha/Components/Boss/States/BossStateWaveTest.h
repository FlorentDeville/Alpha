/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

namespace Systems
{
	class GameObject;
	class MeshAsset;
	class MaterialInstanceAsset;
}

class BossGameObject;
class WaveMachineGun;
class WaveTest;

class BossStateWaveTest : public IState
{
public:
	BossStateWaveTest(StateMachine* pStateMachine);
	~BossStateWaveTest();

	void Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial, 
		BossGameObject* pComponent, const Systems::GameObject* pTarget);

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	WaveTest* m_pWave;
	WaveMachineGun* m_pWaveMachineGun;

	BossGameObject* m_pBoss;
	
	uint32_t m_waveIndex;
	uint32_t m_waveMachineGunIndex;

	bool m_runFirstWave;
};
