/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/ReflectionStandardTypes.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	void RegisterStandardTypesToReflection()
	{
		REGISTER_TYPE(uint8_t);
		REGISTER_TYPE(int8_t);
		REGISTER_TYPE(int32_t);
		REGISTER_TYPE(uint32_t);
		REGISTER_TYPE(uint16_t);
		REGISTER_TYPE(float);
		REGISTER_TYPE(char);
		REGISTER_TYPE(double);
		REGISTER_TYPE(std::string);
	}
}
