/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Bullets/IBulletWave.h"

class WaveTest : public IBulletWave
{
public:
	WaveTest();
	~WaveTest();

	void Spawn(Bullets& bullets, const Core::Vec4f& pos) override;
	void Update(Bullets& bullets, float dt) override;
};
