/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/ReflectionMgr.h"

namespace Systems
{
	ReflectionMgr::ReflectionMgr()
		: m_typeDb()
	{ }

	ReflectionMgr::~ReflectionMgr()
	{ }

	void ReflectionMgr::RegisterType(TypeDescriptor* pType)
	{
		const std::string& typeName = pType->GetName();
		std::map<std::string, TypeDescriptor*>::iterator it = m_typeDb.find(typeName);
		if (it != m_typeDb.end())
			assert(false); //this type is already registered and can't be registered again manually

		m_typeDb[typeName] = pType;
	}

	TypeDescriptor* ReflectionMgr::GetOrAddType(const std::string& name)
	{
		std::map<std::string, TypeDescriptor*>::iterator it = m_typeDb.find(name);
		if (it != m_typeDb.end())
			return it->second;

		TypeDescriptor* pType = new TypeDescriptor(name, 0);
		m_typeDb[name] = pType;
		return pType;
	}

	const TypeDescriptor* ReflectionMgr::GetType(const std::string& name) const
	{
		std::map<std::string, TypeDescriptor*>::const_iterator it = m_typeDb.find(name);
		if (it == m_typeDb.end())
		{
			assert(false && "The type does not exist");
			return nullptr;
		}
			
		return it->second;
	}
}
