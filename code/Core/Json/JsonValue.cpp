/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonValue.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"

namespace Core
{
	JsonValue::JsonValue()
		: m_type(JsonType::Null)
		, m_value()
	{}

	JsonValue::JsonValue(int32_t value)
		: m_type(JsonType::Number)
	{
		m_value = static_cast<double>(value);
	}

	JsonValue::JsonValue(float value)
		: m_type(JsonType::Number)
	{
		m_value = static_cast<double>(value);
	}

	JsonValue::JsonValue(double value)
		: m_type(JsonType::Number)
	{
		m_value = value;
	}

	JsonValue::JsonValue(bool value)
		: m_type(JsonType::Boolean)
	{
		m_value = value;
	}

	JsonValue::JsonValue(const char* pValue)
		: m_type(JsonType::String)
	{
		m_value = pValue;
	}

	JsonValue::JsonValue(const std::string& value)
		: m_type(JsonType::String)
	{
		m_value = value;
	}

	JsonValue::JsonValue(JsonObject* pValue)
		: m_type(JsonType::Object)
	{
		m_value = pValue;
	}

	JsonValue::JsonValue(JsonArray* pValue)
		: m_type(JsonType::Array)
	{
		m_value = pValue;
	}

	JsonValue::~JsonValue()
	{
		if (m_type == JsonType::Array)
		{
			JsonArray* pArray = std::get<JsonArray*>(m_value);
			delete pArray;
			m_value = static_cast<JsonArray*>(nullptr);
		}
		else if (m_type == JsonType::Object)
		{
			JsonObject* pObject = std::get<JsonObject*>(m_value);
			delete pObject;
			m_value = static_cast<JsonObject*>(nullptr);
		}
	}

	JsonType JsonValue::GetType() const
	{
		return m_type;
	}

	bool JsonValue::GetValueAsBool() const
	{
		return std::get<bool>(m_value);
	}

	double JsonValue::GetValueAsDouble() const
	{
		return std::get<double>(m_value);
	}

	std::string JsonValue::GetValueAsString() const
	{
		return std::get<std::string>(m_value);
	}

	JsonArray* JsonValue::GetValueAsArray() const
	{
		return std::get<JsonArray*>(m_value);
	}

	JsonObject* JsonValue::GetValueAsObject() const
	{
		return std::get<JsonObject*>(m_value);
	}
}
