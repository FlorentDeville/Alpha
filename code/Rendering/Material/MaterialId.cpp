/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Material/MaterialId.h"

namespace Rendering
{
	MaterialId::MaterialId()
	{
		m_id = UINT32_MAX; //invalid by default
	}

	MaterialId::MaterialId(uint32_t id)
		: m_id(id)
	{}

	const MaterialId MaterialId::INVALID = MaterialId(); //the default is -1
}
