/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/TypeDescriptor.h"

#include "Systems/Reflection/FieldDescriptor.h"

namespace Systems
{
	TypeDescriptor::TypeDescriptor(const std::string& name, uint64_t size)
		: m_name(name)
		, m_size(size)
		, m_pBaseType(nullptr)
	{
		m_sid = Core::MakeSid(name);
	}

	FieldDescriptor* TypeDescriptor::AddField()
	{
		m_fields.push_back(FieldDescriptor());
		FieldDescriptor& field = m_fields.back();
		return &field;
	}

	void TypeDescriptor::SetBaseType(const std::string& baseTypeName)
	{
		ReflectionMgr& reflectionMgr = ReflectionMgr::Get();
		TypeDescriptor* pBaseType = reflectionMgr.GetOrAddType(baseTypeName);
		m_pBaseType = pBaseType;
	}

	const std::string& TypeDescriptor::GetName() const
	{
		return m_name;
	}

	Core::Sid TypeDescriptor::GetSid() const
	{
		return m_sid;
	}

	const std::vector<FieldDescriptor>& TypeDescriptor::GetFields() const
	{
		return m_fields;
	}

	const TypeDescriptor* TypeDescriptor::GetBaseType() const
	{
		return m_pBaseType;
	}

	bool TypeDescriptor::IsObject() const
	{
		const TypeDescriptor* pType = this;
		while (pType)
		{
			if (pType->m_sid == MAKESID("Systems::Object"))
				return true;

			pType = pType->m_pBaseType;
		}

		return false;
	}

	bool TypeDescriptor::IsClass() const
	{
		return m_fields.size() > 0;
	}
}
