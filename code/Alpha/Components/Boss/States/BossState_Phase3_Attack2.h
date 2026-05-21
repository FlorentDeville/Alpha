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

class BossState_Phase3_Attack2 : public IState
{
public:
	BossState_Phase3_Attack2(StateMachine* pStateMachine);
	~BossState_Phase3_Attack2();

	void Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial,
		BossGameObject* pBoss, const Systems::GameObject* pTarget);

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	static const uint32_t WAVE_COUNT = 10;
	WaveMachineGun* m_pWave[WAVE_COUNT];

	BossGameObject* m_pBoss;
	const Systems::GameObject* m_pTarget;

	uint32_t m_waveIndex[WAVE_COUNT];
	uint32_t m_nextWaveToSpawn;
	float m_lastWaveSpawnTime;
};
