/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	extern int s_componentCounter;

	template <class T> int GetComponentId()
	{
		static int s_componentId = s_componentCounter++;
		return s_componentId;
	}
}
