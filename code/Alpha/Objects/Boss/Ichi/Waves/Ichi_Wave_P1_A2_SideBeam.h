/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

#include "Core/Math/Vec4f.h"

class IchiWaveP1A2SideBeam : public IBulletWave
{
public:
	IchiWaveP1A2SideBeam(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount = 10);
	~IchiWaveP1A2SideBeam();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets) override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;

	void SetSpawnPosition(const Core::Vec4f& spawnPosition);
	void SetSpawnSpeed(const Core::Vec4f& spawnSpeed);

	void DisableSpawn();

private:

	//parameters that can be modified at runtime
	Core::Vec4f m_spawnPosition;
	Core::Vec4f m_spawnSpeed;

	//internal variables

	enum State
	{
		WARMUP,	//the bullets appear progressiely
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

	void SpawnBullet(Bullets& bullets);
};
