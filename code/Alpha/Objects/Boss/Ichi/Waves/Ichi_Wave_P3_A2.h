/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

#include "Core/Math/Vec4f.h"

// Shoots bullets from 2 spawning points alternatively.
class IchiWaveP3A2 : public IBulletWave
{
public:
	IchiWaveP3A2(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount = 10);
	~IchiWaveP3A2();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets) override;
	void Stop() override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;

	void SetSpawnPositionsCount(uint8_t count);
	void SetSpawnPosition(uint8_t index, const Core::Vec4f& position);
	void SetSpawnSpeed(const Core::Vec4f& spawnSpeed);

private:

	//parameters that can be modified at runtime
	uint8_t m_spawnPositionCount;
	Core::Vec4f* m_pSpawnPositions;
	Core::Vec4f m_spawnSpeed;

	//internal variables

	enum State
	{
		WARMUP,				//the bullets appear progressiely
		FIRE,				//the bullets shoot through the level
		WAIT_FOR_NEXT_WAVE, //wait for the next wave of bullet to warmup

		STOP,				//stop spawning bullets, but still updates them

		COUNT
	};

	float m_internalStateStartTime;

	float m_currentScale;

	State m_currentState;

	uint32_t m_nextBulletToSpawn;

	bool m_enableSpawn;

	void SpawnBullet(Bullets& bullets);
};
