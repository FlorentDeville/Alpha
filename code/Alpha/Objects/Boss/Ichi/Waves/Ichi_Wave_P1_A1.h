/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

class IchiWaveP1A1 : public IBulletWave
{
public:
	IchiWaveP1A1(Systems::MeshAsset* pMesh, Systems::MaterialInstanceAsset* pMaterial, uint32_t bulletCount = 10, float rotationOffset = 0);
	~IchiWaveP1A1();

	void Init(Bullets& bullets) override;
	void Destroy(Bullets& bullets) override;
	void Start(Bullets& bullets, const Core::Vec4f& pos) override;
	void Update(Bullets& bullets, float dt) override;
	void BuildRenderable(Bullets& bullets, Systems::RenderableScene& scene) override;

private:

	float m_rotationOffset; //start location rotation offset in radian

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
};
