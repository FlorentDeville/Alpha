/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Sid/Sid.h"

#include "Core/String/BytesToHexa.h"

namespace Core
{
	std::string ToString(Sid sid)
	{
		return Uint64ToHexa(sid);
	}
}
