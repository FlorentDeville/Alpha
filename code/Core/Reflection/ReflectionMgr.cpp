/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Reflection/ReflectionMgr.h"

#include <assert.h>

namespace Core
{
	ReflectionMgr::ReflectionMgr()
		: m_typeDb()
	{ }

	ReflectionMgr::~ReflectionMgr()
	{
		for (std::pair<const std::string, Core::TypeDescriptor*>& pair : m_typeDb)
		{
			delete pair.second;
		}

		m_typeDb.clear();
		m_sidToType.clear();
	}

	void ReflectionMgr::RegisterType(Core::TypeDescriptor* pType)
	{
		const std::string& typeName = pType->GetName();
		std::map<std::string, Core::TypeDescriptor*>::iterator it = m_typeDb.find(typeName);
		if (it != m_typeDb.end())
			assert(false); //this type is already registered and can't be registered again manually

		m_typeDb[typeName] = pType;
		m_sidToType[pType->GetSid()] = pType;
	}

	Core::TypeDescriptor* ReflectionMgr::GetOrAddType(const std::string& name)
	{
		std::map<std::string, Core::TypeDescriptor*>::iterator it = m_typeDb.find(name);
		if (it != m_typeDb.end())
			return it->second;

		Core::TypeDescriptor* pType = new Core::TypeDescriptor(name);
		m_typeDb[name] = pType;
		m_sidToType[SID(name)] = pType;

		return pType;
	}

	const Core::TypeDescriptor* ReflectionMgr::GetType(const std::string& name) const
	{
		std::map<std::string, Core::TypeDescriptor*>::const_iterator it = m_typeDb.find(name);
		if (it == m_typeDb.end())
		{
			assert(false && "The type does not exist");
			return nullptr;
		}
			
		return it->second;
	}

	const Core::TypeDescriptor* ReflectionMgr::GetType(const Core::Sid sidType) const
	{
		std::map<Core::Sid, Core::TypeDescriptor*>::const_iterator it = m_sidToType.find(sidType);
		if (it == m_sidToType.end())
		{
			assert(false && "The type does not exist");
			return nullptr;
		}

		return it->second;
	}

	void ReflectionMgr::ForEachType(const std::function<void(const Core::TypeDescriptor*)>& callback) const
	{
		for (const std::pair<Core::Sid, Core::TypeDescriptor*>& pair : m_sidToType)
		{
			callback(pair.second);
		}
	}
}
