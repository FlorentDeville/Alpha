/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

class WaveTest : public IBulletWave
{
public:
	WaveTest(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial);
	~WaveTest();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets, const Core::Vec4f& pos) override;
	void Stop() override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene);
};
