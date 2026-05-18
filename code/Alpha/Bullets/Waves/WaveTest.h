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
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;

private:
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
