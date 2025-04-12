/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	DEFINE_TYPE_RESOLVER(int32_t)
	DEFINE_TYPE_RESOLVER(uint16_t)
	DEFINE_TYPE_RESOLVER(double)
	DEFINE_TYPE_RESOLVER(float)
	DEFINE_TYPE_RESOLVER(bool)
	DEFINE_TYPE_RESOLVER(std::string)

	template<typename T> class TypeResolver<std::vector<T>>
	{ 
	public:
		static const std::string& GetTypename() { static const std::string typeName = "std::vector"; return typeName; }
		static const TypeDescriptor* GetConstType() { return Systems::ReflectionMgr::Get().GetType(GetTypename()); }
		static TypeDescriptor* GetType() { return Systems::ReflectionMgr::Get().GetType(GetTypename()); }
	};

	// Register the standard c++ types like bool, float ,etc
	void RegisterStandardTypesToReflection();
}
