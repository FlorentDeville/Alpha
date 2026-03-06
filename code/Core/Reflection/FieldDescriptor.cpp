/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Reflection/FieldDescriptor.h"

namespace Core
{
	FieldDescriptor::FieldDescriptor()
		: m_name()
		, m_offset(0)
		, m_pType(nullptr)
		, m_attribute(Core::FieldAttribute::None)
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
		bool hidden = m_attribute & Core::FieldAttribute::Hidden;
		return hidden;
	}

	bool FieldDescriptor::IsSerialized() const
	{
		bool doNotSerialized = m_attribute & Core::FieldAttribute::DoNotSerialize;
		return !doNotSerialized;
	}

	bool FieldDescriptor::IsReadOnly() const
	{
		bool readOnly = m_attribute & Core::FieldAttribute::ReadOnly;
		return readOnly;
	}
}
