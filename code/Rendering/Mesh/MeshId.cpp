/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Mesh/MeshId.h"

#include <cstdint>

namespace Rendering
{
	MeshId::MeshId()
	{
		m_id = UINT64_MAX; //invalid by default
	}

	MeshId::MeshId(size_t id)
		: m_id(id)
	{}

	bool MeshId::operator==(const MeshId id) const
	{
		return id.m_id == m_id;
	}

	bool MeshId::operator!=(const MeshId id) const
	{
		return id.m_id != m_id;
	}

	const MeshId MeshId::INVALID = MeshId(); //the default is -1
}
