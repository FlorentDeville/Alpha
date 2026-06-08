/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

#include "Core/Math/Vec4f.h"

namespace Systems
{
	class GameObject;
}

class WaveMachineGun : public IBulletWave
{
	using BaseClass = IBulletWave;

public:
	WaveMachineGun(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial,
		Systems::GameObject* pOwner, const Systems::GameObject* pTarget);
	~WaveMachineGun();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets) override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;
	void CollisionDetection(Bullets& bullet) override;

	void SetSideBulletEnabled(bool enabled);
	void SetSideBulletCount(uint32_t count);
	void SetBulletCount(uint32_t count);
	void SetCounterableBulletCount(uint32_t count);
	void SetGapTime(float gapTime);
	void SetSpeed(float speed);

private:
	Systems::GameObject* m_pOwner;
	const Systems::GameObject* m_pTarget;

	Systems::MaterialInstanceAsset* m_pCounterBulletMaterial;

	uint32_t m_nextBulletToShot;
	float m_lastSpawnTime;

	uint32_t m_counterableBulletCount;

	float m_gapTime; 
	bool m_sideBulletEnabled;
	float m_speed;
	uint32_t m_sideBulletCount;
};

