/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/TypeDescriptor.h"

#include "Systems/Reflection/FieldDescriptor.h"

namespace Systems
{
	uint64_t TypeDescriptor::s_idCounter = 1;

	TypeDescriptor::TypeDescriptor(const std::string& name, uint64_t size)
		: m_name(name)
		, m_size(size)
		, m_pBaseType(nullptr)
		, m_id(++s_idCounter)
	{ }

	FieldDescriptor* TypeDescriptor::AddField()
	{
		m_fields.push_back(FieldDescriptor());
		FieldDescriptor& field = m_fields.back();
		return &field;
	}

	void TypeDescriptor::SetBaseType(const std::string& baseTypeName)
	{
		ReflectionMgr& reflectionMgr = ReflectionMgr::Get();
		TypeDescriptor* pBaseType = reflectionMgr.GetType(baseTypeName);
		m_pBaseType = pBaseType;
	}

	const std::string& TypeDescriptor::GetName() const
	{
		return m_name;
	}
}
