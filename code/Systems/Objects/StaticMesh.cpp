/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Objects/StaticMesh.h"

namespace Systems
{
	const MeshAsset* StaticMesh::GetMesh() const
	{
		return m_mesh.GetPtr();
	}

	const MaterialInstanceAsset* StaticMesh::GetMaterialInstance() const
	{
		return m_material.GetPtr();
	}
}
