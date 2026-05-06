/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/IBulletWave.h"

IBulletWave::IBulletWave()
	: m_startId(0)
	, m_endId(0)
	, m_pMesh(nullptr)
	, m_pMaterial(nullptr)
{ }

IBulletWave::~IBulletWave()
{ }
