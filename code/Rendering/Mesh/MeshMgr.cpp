/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Mesh/MeshMgr.h"

#include "Rendering/Mesh/Mesh.h"

#include <assert.h>

namespace Rendering
{
	MeshMgr::MeshMgr()
		: Core::Singleton<MeshMgr>()
		, m_meshes()
	{}

	MeshMgr::~MeshMgr()
	{
		for (const Mesh* pMesh : m_meshes)
			delete pMesh;
	}

	bool MeshMgr::CreateMesh(Mesh** ppMesh, MeshId& id)
	{
		id = MeshId(m_meshes.size());
		m_meshes.push_back(new Mesh());
		*ppMesh = m_meshes.back();

		return true;
	}

	const Mesh* MeshMgr::GetMesh(MeshId id) const
	{
		assert(id.m_id < m_meshes.size());
		return m_meshes[id.m_id];
	}
}
