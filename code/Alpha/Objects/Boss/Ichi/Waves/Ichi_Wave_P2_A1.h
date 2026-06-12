/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

#include "Core/Math/Vec4f.h"

class IchiWaveP2A1 : public IBulletWave
{
public:
	IchiWaveP2A1(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount = 10);
	~IchiWaveP2A1();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets) override;
	void Stop() override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;

	void SetSpawnPosition(const Core::Vec4f& p1, const Core::Vec4f& p2);
	void SetSpawnSpeed(const Core::Vec4f& spawnSpeed);

private:

	//parameters that can be modified at runtime
	static const uint8_t SPAWN_POSITION_COUNT = 2;
	Core::Vec4f m_spawnPosition[SPAWN_POSITION_COUNT];
	Core::Vec4f m_spawnSpeed;

	//internal variables

	enum State
	{
		//WARMUP,	//the bullets appear progressiely
		FIRE,	//the bullets shoot through the level

		COUNT
	};

	//warmup state
	float m_warmupDuration;
	float m_warmupElapsedTime;

	float m_currentScale;

	State m_currentState;

	uint32_t m_nextBulletToSpawn;

	float m_lastBulletSpawnedTime;

	bool m_enableSpawn;
	uint8_t m_spawnIndex;

	void SpawnBullet(Bullets& bullets);
};
