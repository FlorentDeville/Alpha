/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class Vec4f;
}

namespace Systems
{
	class MaterialInstanceAsset;
	class MeshAsset;
	class RenderableScene;
}

class Bullets;

class IBulletWave
{
public:
	IBulletWave();
	virtual ~IBulletWave();

	virtual void Init(Bullets& bullets) = 0;
	virtual void Destroy(Bullets& bullets) = 0;

	virtual void Start(Bullets& bullets, const Core::Vec4f& pos) = 0;
	virtual void Stop() = 0;
	virtual void Update(Bullets& bullets, float dt) = 0;
	virtual void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) = 0;
	virtual void CollisionDetection(Bullets& bullet);

	bool IsAlive() const;

protected:
	uint32_t m_count;

	uint32_t m_startId;
	uint32_t m_endId; //id of the last bullet (non inclusive : the bullet at id m_endId is not used by this wave)

	float m_counterBulletCollisionRadius; //radius of the collision for counter bullet

	bool m_isAlive;

	Systems::MeshAsset* m_pMesh;
	Systems::MaterialInstanceAsset* m_pMaterial;

	bool CollisionTestForBullet(const Bullets& bullets, uint32_t index);
	bool CollisionTestForBulletCounter(const Bullets& bullets, uint32_t index);
};
