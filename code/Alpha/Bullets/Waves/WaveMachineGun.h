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
		const Systems::GameObject* pOwner, const Systems::GameObject* pTarget);
	~WaveMachineGun();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets, const Core::Vec4f& pos) override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;
	void CollisionDetection(Bullets& bullet) override;

	void SpawnCounterBullet(Bullets& bullet, uint32_t index) override;

private:
	const Systems::GameObject* m_pOwner;
	const Systems::GameObject* m_pTarget;

	Systems::MaterialInstanceAsset* m_pCounterBulletMaterial;

	uint32_t m_nextBulletToShot;
	float m_lastSpawnTime;

	const uint32_t COUNTER_BULLET_COUNT;

	uint32_t m_counterBulletStartId;
	uint32_t m_counterBulletEndId;
	uint32_t m_nextCounterBulletId;

	struct CounterBulletStruct
	{
		Core::Vec4f m_bezierP0;
		Core::Vec4f m_bezierP1;
	};
	CounterBulletStruct* m_pCounterBulletState;

	void UpdateCounteredBullets(Bullets& bullets, float dt);
};

