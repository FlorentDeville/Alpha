/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonObject.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonMember.h"

namespace Core
{
	JsonObject::JsonObject()
		: m_members()
	{}

	JsonObject::~JsonObject()
	{
		for (JsonMember* pMember : m_members)
			delete pMember;

		m_members.clear();
	}

	void JsonObject::AddMember(const std::string& name, int32_t value)
	{
		JsonMember* pNewMember = new JsonMember(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, float value)
	{
		JsonMember* pNewMember = new JsonMember(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, bool value)
	{
		JsonMember* pNewMember = new JsonMember(name, value);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, const char* pValue)
	{
		JsonMember* pNewMember = new JsonMember(name, pValue);
		m_members.push_back(pNewMember);
	}

	void JsonObject::AddMember(const std::string& name, const std::string& value)
	{
		JsonMember* pNewMember = new JsonMember(name, value);
		m_members.push_back(pNewMember);
	}

	JsonObject* JsonObject::AddObject(const std::string& name)
	{
		JsonObject* pNewObject = new JsonObject();

		JsonMember* pNewMember = new JsonMember(name, pNewObject);
		m_members.push_back(pNewMember);

		return pNewObject;
	}

	JsonArray* JsonObject::AddArray(const std::string& name)
	{
		JsonArray* pNewArray = new JsonArray();
		
		JsonMember* pNewMember = new JsonMember(name, pNewArray);
		m_members.push_back(pNewMember);

		return pNewArray;
	}

	const std::vector<JsonMember*>& JsonObject::GetMembers() const
	{
		return m_members;
	}
}