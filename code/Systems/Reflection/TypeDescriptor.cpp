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
		, m_upgradeType(Core::INVALID_SID)
	{
		Init(name, size);
	}

	void TypeDescriptor::Init(const std::string& name, uint64_t size)
	{
		m_name = name;
		m_size = size;
		m_pBaseType = nullptr;
		m_upgradeType = Core::INVALID_SID;
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

	const std::vector<FieldDescriptor>& TypeDescriptor::GetFields() const
	{
		return m_fields;
	}

	const TypeDescriptor* TypeDescriptor::GetBaseType() const
	{
		return m_pBaseType;
	}

	Core::Sid TypeDescriptor::GetUpgradeType() const
	{
		return m_upgradeType;
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
}
