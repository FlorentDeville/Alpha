/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Systems
{
	class TypeDescriptor;

	template<typename T> class TypeResolver
	{
	public:
		static const std::string& GetTypename();
		static const TypeDescriptor* GetConstType();
		static TypeDescriptor* GetType();
	};
}
