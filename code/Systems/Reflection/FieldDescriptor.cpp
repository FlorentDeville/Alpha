/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/FieldDescriptor.h"

namespace Systems
{
	FieldDescriptor::FieldDescriptor()
		: m_name()
		, m_offset(0)
		, m_pType(nullptr)
		, m_attribute(FieldAttribute::None)
		, m_isPointer(false)
	{ }

	FieldDescriptor::~FieldDescriptor()
	{ }

	const std::string& FieldDescriptor::GetName() const
	{
		return m_name;
	}

	const TypeDescriptor* FieldDescriptor::GetType() const
	{
		return m_pType;
	}

	bool FieldDescriptor::IsPointer() const
	{
		return m_isPointer;
	}

	const void* FieldDescriptor::GetDataPtr(const void* pObj) const
	{
		return reinterpret_cast<const char*>(pObj) + m_offset;
	}

	void* FieldDescriptor::GetDataPtr(void* pObj) const
	{
		return reinterpret_cast<char*>(pObj) + m_offset;
	}

	bool FieldDescriptor::IsHidden() const
	{
		bool hidden = m_attribute & FieldAttribute::Hidden;
		return hidden;
	}

	bool FieldDescriptor::IsReadOnly() const
	{
		bool readOnly = m_attribute & FieldAttribute::ReadOnly;
		return readOnly;
	}
}
