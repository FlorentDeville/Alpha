/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonObject.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonMember.h"
#include "Core/Memory/MemoryMacro.h"

#include <assert.h>

namespace Core
{
	JsonObject::JsonObject()
		: m_members()
	{}

	JsonObject::~JsonObject()
	{
		for (JsonMember* pMember : m_members)
			DELETE(pMember);

		m_members.clear();
	}

	void JsonObject::AddMember(const std::string& name, int32_t value)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, uint64_t value)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, float value)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, double value)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, bool value)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, const char* pValue)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, pValue);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, const std::string& value)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, value);
		m_members.push_back(pNewMember);
	}

	JsonValue* JsonObject::AddMember(const std::string& name)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name);
		m_members.push_back(pNewMember);
		return &(pNewMember->GetValue());
	}

	void JsonObject::AddNullMember(const std::string& name)
	{
		JsonMember* pNewMember = MAKENEW(JsonMember)(name);
		m_members.push_back(pNewMember);
	}

	JsonObject* JsonObject::AddObject(const std::string& name)
	{
		JsonObject* pNewObject = MAKENEW(JsonObject)();

		JsonMember* pNewMember = MAKENEW(JsonMember)(name, pNewObject);
		m_members.push_back(pNewMember);

		return pNewObject;
	}

	JsonArray* JsonObject::AddArray(const std::string& name)
	{
		JsonArray* pNewArray = MAKENEW(JsonArray)();
		
		JsonMember* pNewMember = MAKENEW(JsonMember)(name, pNewArray);
		m_members.push_back(pNewMember);

		return pNewArray;
	}

	const std::vector<JsonMember*>& JsonObject::GetMembers() const
	{
		return m_members;
	}

	const JsonValue& JsonObject::GetMember(const std::string& name) const
	{
		//Booo!! This is bad. I could make a map, or use a sid for member's name.
		for (const JsonMember* member : m_members)
		{
			if (member->GetName() == name)
				return member->GetConstValue();
		}

		assert(false);
		
		static JsonValue DummyValue;
		return DummyValue;
	}

	const JsonValue& JsonObject::GetMember(int32_t index) const
	{
		return m_members[index]->GetConstValue();
	}
}