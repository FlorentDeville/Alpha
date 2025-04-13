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
		, m_pElementType(nullptr)
		, m_attribute(FieldAttribute::None)
		, m_isPointer(false)
		, m_isContainer(false)
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

	const TypeDescriptor* FieldDescriptor::GetElementType() const
	{
		return m_pElementType;
	}

	bool FieldDescriptor::IsPointer() const
	{
		return m_isPointer;
	}

	bool FieldDescriptor::IsContainer() const
	{
		return m_isContainer;
	}

	bool FieldDescriptor::IsElementPointer() const
	{
		return m_isElementPointer;
	}

	const void* FieldDescriptor::GetDataPtr(const void* pObj) const
	{
		return reinterpret_cast<const char*>(pObj) + m_offset;
	}

	void* FieldDescriptor::GetDataPtr(void* pObj) const
	{
		return reinterpret_cast<char*>(pObj) + m_offset;
	}
}
