/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Sid/Sid.h"
#include "Core/Singleton.h"
#include "Core/Reflection/TypeDescriptor.h"

#include <functional>
#include <map>
#include <string>

namespace Core
{
	class ReflectionMgr : public Core::Singleton<ReflectionMgr>
	{
	public:
		ReflectionMgr();
		~ReflectionMgr();

		template<typename T> Core::TypeDescriptor* RegisterType(const std::string& name);
		void RegisterType(Core::TypeDescriptor* pType);

		Core::TypeDescriptor* GetOrAddType(const std::string& name);

		const Core::TypeDescriptor* GetType(const std::string& name) const;
		const Core::TypeDescriptor* GetType(const Core::Sid sidType) const;

		void ForEachType(const std::function<void(const Core::TypeDescriptor*)>& callback) const;

	private:
		std::map<std::string, Core::TypeDescriptor*> m_typeDb;
		std::map<Core::Sid, Core::TypeDescriptor*> m_sidToType;
	};

	template<typename T> Core::TypeDescriptor* ReflectionMgr::RegisterType(const std::string& name)
	{
		Core::TypeDescriptor* pType = nullptr;

		std::map<std::string, Core::TypeDescriptor*>::iterator it = m_typeDb.find(name);
		if (it == m_typeDb.end())
		{
			pType = new Core::TypeDescriptor(name);
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
