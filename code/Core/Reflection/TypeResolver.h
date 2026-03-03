/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

namespace Systems
{
	class TypeDescriptor;
}

namespace Core
{
	class TypeDescriptor;

	template<typename T> class TypeResolver
	{
	public:
		static const std::string& GetTypename();
		static const Systems::TypeDescriptor* GetConstType();
		static Systems::TypeDescriptor* GetType();
	};
}
