/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	template<typename T> struct IsPointer { static const bool value = false; };
	template<typename T> struct IsPointer<T*> { static const bool value = true; };

	template<typename T> struct RemovePointer { typedef T type; };
	template<typename T> struct RemovePointer<T*> { typedef T type; };
}
