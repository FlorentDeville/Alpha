/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Systems/Reflection/TypeDescriptor.h"

#include <assert.h>
#include <map>
#include <string>

namespace Systems
{
	class TypeDescriptor;

	class ReflectionMgr : public Core::Singleton<ReflectionMgr>
	{
	public:
		ReflectionMgr();
		~ReflectionMgr();

		template<typename T> TypeDescriptor* RegisterType(const std::string& name);

		template<typename T> const std::string& GetTypename() const;

		void RegisterType(TypeDescriptor* pType);

		TypeDescriptor* GetType(const std::string& name);

	private:
		std::map<std::string, TypeDescriptor*> m_typeDb;
	};

	template<typename T> TypeDescriptor* ReflectionMgr::RegisterType(const std::string& name)
	{
		std::map<std::string, TypeDescriptor*>::iterator it = m_typeDb.find(name);
		if (it == m_typeDb.end())
		{
			TypeDescriptor* pType = new TypeDescriptor(name, sizeof(T));
			m_typeDb[name] = pType;
			return pType;
		}
		else
		{
			TypeDescriptor* pType = it->second;
			new (pType) TypeDescriptor(name, sizeof(T));
			return pType;
		}
	}

	template<typename T> const std::string& ReflectionMgr::GetTypename() const
	{
		assert(false); //the default assert, it needs to be specialized for every type
	}
}
