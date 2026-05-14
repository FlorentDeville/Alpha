/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

namespace Systems
{
	class GameObject;
}

class WaveMachineGun : public IBulletWave
{
public:
	WaveMachineGun(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, Systems::MaterialInstanceAsset* pCounterBulletMaterial, 
		const Systems::GameObject* pOwner, const Systems::GameObject* pTarget);
	~WaveMachineGun();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets, const Core::Vec4f& pos) override;
	void Stop() override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;

private:
	const Systems::GameObject* m_pOwner;
	const Systems::GameObject* m_pTarget;

	Systems::MaterialInstanceAsset* m_pCounterBulletMaterial;

	uint32_t m_nextBulletToShot;
	float m_lastSpawnTime;
};
