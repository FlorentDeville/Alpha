/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>
#include <vector>

#include "Core/Singleton.h"
#include "Rendering/Mesh/MeshId.h"
 
namespace Rendering
{

	class Mesh;

	class MeshMgr : public Core::Singleton<MeshMgr>
	{
	public:
		MeshMgr();
		~MeshMgr();

		bool CreateMesh(Mesh** ppMesh, MeshId& id);

		const Mesh* GetMesh(MeshId id) const;

	private:
		std::vector<Mesh*> m_meshes;
	};
}
