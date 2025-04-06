/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonArray.h"

#include "Core/Json/JsonValue.h"

namespace Core
{
	JsonArray::JsonArray()
		: m_elements()
	{}

	JsonArray::~JsonArray()
	{
		for (JsonValue* pValue : m_elements)
			delete pValue;

		m_elements.clear();
	}

	void JsonArray::AddElement(int32_t value)
	{
		JsonValue* pNewValue = new JsonValue(value);
		m_elements.push_back(pNewValue);
	}

	void JsonArray::AddElement(float value)
	{
		JsonValue* pNewValue = new JsonValue(value);
		m_elements.push_back(pNewValue);
	}

	void JsonArray::AddElement(bool value)
	{
		JsonValue* pNewValue = new JsonValue(value);
		m_elements.push_back(pNewValue);
	}

	void JsonArray::AddElement(const std::string& value)
	{
		JsonValue* pNewValue = new JsonValue(value);
		m_elements.push_back(pNewValue);
	}

	void JsonArray::AddElement(JsonObject* pValue)
	{
		JsonValue* pNewValue = new JsonValue(pValue);
		m_elements.push_back(pNewValue);
	}

	void JsonArray::AddElement(JsonArray* pValue)
	{
		JsonValue* pNewValue = new JsonValue(pValue);
		m_elements.push_back(pNewValue);
	}

	const std::vector<JsonValue*>& JsonArray::GetElements() const
	{
		return m_elements;
	}
}
