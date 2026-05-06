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

	void Update(float dt) override;
};
