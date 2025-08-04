/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"
#include "Core/Singleton.h"

#include "Systems/Reflection/TypeDescriptor.h"

#include <functional>
#include <map>
#include <string>

namespace Systems
{
	class ReflectionMgr : public Core::Singleton<ReflectionMgr>
	{
	public:
		ReflectionMgr();
		~ReflectionMgr();

		template<typename T> TypeDescriptor* RegisterType(const std::string& name);
		void RegisterType(TypeDescriptor* pType);

		TypeDescriptor* GetOrAddType(const std::string& name);

		const TypeDescriptor* GetType(const std::string& name) const;
		const TypeDescriptor* GetType(const Core::Sid sidType) const;

		void ForEachType(const std::function<void(const TypeDescriptor*)>& callback) const;

	private:
		std::map<std::string, TypeDescriptor*> m_typeDb;
		std::map<Core::Sid, TypeDescriptor*> m_sidToType;
	};

	template<typename T> TypeDescriptor* ReflectionMgr::RegisterType(const std::string& name)
	{
		TypeDescriptor* pType = nullptr;

		std::map<std::string, TypeDescriptor*>::iterator it = m_typeDb.find(name);
		if (it == m_typeDb.end())
		{
			pType = new TypeDescriptor(name);
			m_typeDb[name] = pType;
			m_sidToType[SID(name)] = pType;
		}
		else
		{
			pType = it->second;
		}

		return pType;
	}
}
