/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonSerializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonMember.h"
#include "Core/Json/JsonObject.h"

#include <assert.h>
#include <vector>

namespace Core
{
	void WriteArray(const JsonArray& array, std::stringstream& output, int offset);
	void WriteObject(const JsonObject& input, std::stringstream& output, int offset);

	void WriteBoolean(const JsonValue& value, std::stringstream& output)
	{
		if (value.GetValueAsBool())
			output << "true";
		else
			output << "false";
	}

	void WriteNumber(const JsonValue& value, std::stringstream& output)
	{
		output << value.GetValueAsDouble();
	}

	void WriteString(const JsonValue& value, std::stringstream& output)
	{
		output << "\"" << value.GetValueAsString() << "\"";
	}

	void WriteNull(std::stringstream& output)
	{
		output << "null";
	}

	void WriteValue(const JsonValue& value, std::stringstream& output, int offset)
	{
		switch (value.GetType())
		{
		case JsonType::Boolean:
			WriteBoolean(value, output);
			break;

		case JsonType::Number:
			WriteNumber(value, output);
			break;

		case JsonType::String:
			WriteString(value, output);
			break;

		case JsonType::Array:
			WriteArray(*value.GetValueAsArray(), output, offset);
			break;

		case JsonType::Object:
			WriteObject(*value.GetValueAsObject(), output, offset);
			break;

		case JsonType::Null:
			WriteNull(output);
			break;

		default:
			assert(false);
			break;
		}
	}

	void WriteArray(const JsonArray& array, std::stringstream& output, int offset)
	{
		std::string strOffset;
		for (int ii = 0; ii < offset; ++ii)
			strOffset += "\t";

		output << "[\n";

		bool isFirst = true;

		const std::vector<JsonValue*>& elements = array.GetElements();
		for (const JsonValue* pValue : elements)
		{
			if (isFirst)
				isFirst = false;
			else
				output << ",\n";

			output << strOffset << "\t";
			WriteValue(*pValue, output, offset + 1);
		}

		output << "\n " << strOffset << "]";
	}

	void WriteObject(const JsonObject& input, std::stringstream& output, int offset)
	{
		std::string strOffset;
		for (int ii = 0; ii < offset; ++ii)
			strOffset += "\t";

		output << "{\n";

		bool isFirst = true;
		const std::vector<JsonMember*>& members = input.GetMembers();
		for (const JsonMember* pMember : members)
		{
			if (isFirst)
				isFirst = false;
			else
				output << ", \n";
	
			output << strOffset << "\t\"" << pMember->GetName() << "\": ";
			WriteValue(pMember->GetConstValue(), output, offset+1);
		}

		output << "\n" << strOffset << "}";

	}

	JsonSerializer::JsonSerializer()
		: m_stream()
	{}

	JsonSerializer::~JsonSerializer()
	{}

	bool JsonSerializer::Serialize(const JsonObject& input, std::string& output)
	{
		m_stream.clear();
		WriteObject(input, m_stream, 0);

		output = m_stream.str();
		return true;
	}
}
