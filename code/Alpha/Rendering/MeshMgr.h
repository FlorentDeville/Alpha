/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>
#include <vector>

#include "Mesh.h"

using MeshId = size_t;

class MeshMgr
{
public:
	MeshMgr();
	~MeshMgr();

	bool CreateMesh(Mesh** ppMesh, MeshId& id);

	const Mesh* GetMesh(MeshId id) const;

private:
	std::vector<Mesh*> m_meshes;
};

extern MeshMgr* g_pMeshMgr;
