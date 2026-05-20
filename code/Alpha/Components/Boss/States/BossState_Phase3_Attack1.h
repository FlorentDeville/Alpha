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
class WaveTest;

class BossState_Phase3_Attack1 : public IState
{
public:
	BossState_Phase3_Attack1(StateMachine* pStateMachine);
	~BossState_Phase3_Attack1();

	void Init(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial,
		BossGameObject* pBoss, const Systems::GameObject* pTarget);

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	static const int WAVE_COUNT = 5;

	float m_delayBetweenWave;
	float m_lastWaveStartTime;
	uint32_t m_nextWaveToStart;

	WaveTest* m_pWave[WAVE_COUNT];

	BossGameObject* m_pBoss;
	const Systems::GameObject* m_pTarget;
	uint32_t m_waveIndex[WAVE_COUNT];
};
