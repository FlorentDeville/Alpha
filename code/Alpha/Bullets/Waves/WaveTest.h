/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

class WaveTest : public IBulletWave
{
public:
	WaveTest(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount = 10, float rotationOffset = 0);
	~WaveTest();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets, const Core::Vec4f& pos) override;
	void Stop() override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;

private:

	float m_rotationOffset; //start location rotation offset in radian

	enum State
	{
		SHOW,	//the bullets appear progressiely
		FIRE,	//the bullets shoot through the level

		COUNT
	};

	//show state
	float m_showDuration;
	float m_showTime;
	float m_currentScale;

	State m_currentState;
};
