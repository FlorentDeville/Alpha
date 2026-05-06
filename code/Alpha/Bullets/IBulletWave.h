/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Systems
{
	class MaterialInstanceAsset;
	class MeshAsset;
}

class IBulletWave
{
public:
	IBulletWave();
	virtual ~IBulletWave();

	virtual void Update(float dt) = 0;

private:
	uint32_t m_startId;
	uint32_t m_endId;

	Systems::MeshAsset* m_pMesh;
	Systems::MaterialInstanceAsset* m_pMaterial;
};
