/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/FieldAttribute.h"
#include "Systems/Reflection/ReflectionMgr.h"

#include <string>

namespace Systems
{
	class TypeDescriptor;

	class FieldDescriptor
	{
	public:
		FieldDescriptor();
		~FieldDescriptor();

		template<typename FIELD_TYPE> void Init(const std::string& name, size_t offset, FieldAttribute attribute);

	private:
		std::string m_name;
		uint64_t m_offset;
		TypeDescriptor* m_pType;
		FieldAttribute m_attribute;
	};

	template<typename FIELD_TYPE> void FieldDescriptor::Init(const std::string& name, size_t offset, FieldAttribute attribute)
	{
		m_name = name;
		m_offset = offset;
		m_attribute = attribute;
		ReflectionMgr& reflectionMgr = ReflectionMgr::Get();
		m_pType = reflectionMgr.GetType<FIELD_TYPE>();
	}
}
