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

class PlayerWaveCountered : public IBulletWave
{
	using BaseClass = IBulletWave;

public:
	PlayerWaveCountered(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::GameObject* pOwner, const Systems::GameObject* pTarget);
	~PlayerWaveCountered();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets) override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;
	//void CollisionDetection(Bullets& bullet) override;

	void SpawnSingleBullet(const Core::Vec4f& startPos);

private:
	Bullets* m_pBullets;

	Systems::GameObject* m_pOwner;
	const Systems::GameObject* m_pTarget;

	Systems::MeshAsset* m_pMesh;
	Systems::MaterialInstanceAsset* m_pMaterial;

	uint32_t FindFreeSlot() const;
};

