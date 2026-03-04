/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Reflection/TypeDescriptor.h"

#include "Core/Reflection/FieldDescriptor.h"

namespace Core
{
	TypeDescriptor::TypeDescriptor(const std::string& name)
		: m_name(name)
		, m_size(0)
		, m_pBaseType(nullptr)
		, m_upgradeType(Core::INVALID_SID)
		, m_isContainer(false)
		, m_isTemplate(false)
		, m_isTemplateParamTypePointer(false)
		, m_isEnum(false)
		, m_pTemplateParamType(nullptr)
		, Construct()
		, InPlaceConstruct()
		, Destruct()
		, Upgrade()
		, Copy()
	{
		m_sid = Core::MakeSid(name);
		m_sidWithoutTemplateParam = m_sid;
	}

	TypeDescriptor::~TypeDescriptor()
	{
		for (FieldDescriptor* pField : m_fields)
			delete pField;

		m_fields.clear();
	}

	bool TypeDescriptor::IsInitialized() const
	{
		return m_size != 0;
	}

	FieldDescriptor* TypeDescriptor::AddField()
	{
		m_fields.push_back(new FieldDescriptor());
		FieldDescriptor* field = m_fields.back();
		return field;
	}

	void TypeDescriptor::AddEntry(int32_t value, const std::string& name)
	{
		EnumEntry newEntry(value, name);
		m_enumEntries.PushBack(newEntry);
	}

	void TypeDescriptor::SetIsEnum()
	{
		m_isEnum = true;
	}

	void TypeDescriptor::SetBaseType(const std::string& baseTypeName)
	{
		ReflectionMgr& reflectionMgr = ReflectionMgr::Get();
		TypeDescriptor* pBaseType = reflectionMgr.GetOrAddType(baseTypeName);
		m_pBaseType = pBaseType;
	}

	void TypeDescriptor::SetUpgradeType(Core::Sid upgradeType)
	{
		m_upgradeType = upgradeType;
	}

	const std::string& TypeDescriptor::GetName() const
	{
		return m_name;
	}

	Core::Sid TypeDescriptor::GetSid() const
	{
		return m_sid;
	}

	Core::Sid TypeDescriptor::GetSidWithoutTemplateParam() const
	{
		return m_sidWithoutTemplateParam;
	}

	const std::vector<FieldDescriptor*>& TypeDescriptor::GetFields() const
	{
		return m_fields;
	}

	const TypeDescriptor* TypeDescriptor::GetBaseType() const
	{
		return m_pBaseType;
	}

	const TypeDescriptor* TypeDescriptor::GetTemplateParamType() const
	{
		return m_pTemplateParamType;
	}

	Core::Sid TypeDescriptor::GetUpgradeType() const
	{
		return m_upgradeType;
	}

	bool TypeDescriptor::IsContainer() const
	{
		return m_isContainer;
	}

	bool TypeDescriptor::IsTemplate() const
	{
		return m_isTemplate;
	}

	bool TypeDescriptor::IsTemplateParamTypePointer() const
	{
		return m_isTemplateParamTypePointer;
	}
	
	uint64_t TypeDescriptor::GetSize() const
	{
		return m_size;
	}

	bool TypeDescriptor::InheritsFrom(Core::Sid baseClassSid) const
	{
		const TypeDescriptor* pType = m_pBaseType;
		while (pType)
		{
			if (pType->m_sid == baseClassSid)
				return true;

			pType = pType->m_pBaseType;
		}

		return false;
	}

	bool TypeDescriptor::IsObject() const
	{
		if (m_sid == CONSTSID("Systems::Object"))
			return true;
		else
			return InheritsFrom(CONSTSID("Systems::Object"));
	}

	bool TypeDescriptor::IsGameObject() const
	{
		if (m_sid == CONSTSID("Systems::GameObject"))
			return true;
		else
			return InheritsFrom(CONSTSID("Systems::GameObject"));
	}

	// True if the class ihnerits from GameComponent.
	bool TypeDescriptor::IsGameComponent() const
	{
		if (m_sid == CONSTSID("Systems::GameComponent"))
			return true;
		else
			return InheritsFrom(CONSTSID("Systems::GameComponent"));
	}

	bool TypeDescriptor::IsClass() const
	{
		return m_fields.size() > 0;
	}

	bool TypeDescriptor::IsEnum() const
	{
		return m_isEnum;
	}
}
