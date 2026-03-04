/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

namespace Core
{
	class TypeDescriptor;

	template<typename T> class TypeResolver
	{
	public:
		static const std::string& GetTypename();
		static const Core::TypeDescriptor* GetConstType();
		static Core::TypeDescriptor* GetType();
	};
}
