/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"
#include "Core/Sid/Sid.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	DEFINE_TYPE_RESOLVER(Core::Vec4f)
	DEFINE_TYPE_RESOLVER(Core::Mat44f)
	DEFINE_TYPE_RESOLVER(Core::Sid)

	template<typename T> class TypeResolver<Core::Array<T>>
	{
	public:
		static const std::string& GetTypename() { static const std::string typeName = "Core::Array"; return typeName; }
		static const TypeDescriptor* GetConstType() { return Systems::ReflectionMgr::Get().GetOrAddType(GetTypename()); }
		static TypeDescriptor* GetType() { return Systems::ReflectionMgr::Get().GetOrAddType(GetTypename()); }
	};

	void RegisterCoreTypesToReflection();
}
