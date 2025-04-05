/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonMember.h"

namespace Core
{
	JsonMember::JsonMember()
		: m_name()
		, m_value()
	{}

	JsonMember::JsonMember(const std::string& name, int32_t value)
		: m_name(name)
		, m_value(value)
	{}

	JsonMember::JsonMember(const std::string& name, float value)
		: m_name(name)
		, m_value(value)
	{}

	JsonMember::JsonMember(const std::string& name, const char* pValue)
		: m_name(name)
		, m_value(std::string(pValue))
	{
	}

	JsonMember::JsonMember(const std::string& name, const std::string& value)
		: m_name(name)
		, m_value(value)
	{}

	JsonMember::JsonMember(const std::string& name, bool value)
		: m_name(name)
		, m_value(value)
	{}

	JsonMember::JsonMember(const std::string& name, JsonObject* pValue)
		: m_name(name)
		, m_value(pValue)
	{}

	JsonMember::JsonMember(const std::string& name, JsonArray* pValue)
		: m_name(name)
		, m_value(pValue)
	{}

	JsonMember::~JsonMember()
	{}

	const std::string& JsonMember::GetName() const
	{
		return m_name;
	}

	const JsonValue& JsonMember::GetValue() const
	{
		return m_value;
	}
}
