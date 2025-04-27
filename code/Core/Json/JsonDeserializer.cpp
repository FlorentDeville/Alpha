/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonDeserializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"

#include <assert.h>
#include <charconv>
#include <string_view>

namespace Core
{
	std::string UnescapeString(const std::string& source)
	{
		std::string output;
		for (int ii = 0; ii < source.size(); ++ii)
		{
			char c = source[ii];

			if (c == '\\')
			{
				++ii;
				output += source[ii];
			}
			else
			{
				output += c;
			}
		}

		return output;
	}

	void StripLeadingAndTrailingWhitespace(std::string_view& str)
	{
		size_t firstNonWhitespace = str.find_first_not_of(" \r\n\t");
		size_t lastNonWhitespace = str.find_last_not_of(" \r\n\t");

		str = str.substr(firstNonWhitespace, lastNonWhitespace - firstNonWhitespace + 1);
	}

	JsonDeserializer::JsonDeserializer()
		: m_document()
		, m_token()
	{}

	JsonDeserializer::~JsonDeserializer()
	{}

	bool JsonDeserializer::Deserialize(const std::string& input, JsonObject& output)
	{
		m_document = StringView(input.c_str(), static_cast<uint32_t>(input.size()));

		bool continueReading = true;
		while (continueReading)
		{
			continueReading = ReadNextToken();

			if (m_token == "{")
				ReadObject(output);
			else
				assert(false);

			continueReading = ReadNextToken();
		}
		
		return true;
	}

	bool JsonDeserializer::ReadNextToken()
	{
		if(m_document.Empty())
		{
			m_token.Clear();
			return false;
		}

		bool readNextChar = true;
		while (readNextChar)
		{
			char nextChar = m_document.Front();
			switch (nextChar)
			{
			case '\n':
			case '\r':
			case '\t':
			case ' ':
			{
				m_document = m_document.Substr(1);
			}
			break;

			case '{':
			case '}':
			case ':':
			case '[':
			case ']':
			case ',':
			{
				m_token = nextChar;
				m_document = m_document.Substr(1);
				readNextChar = false;
			}
			break;

			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				int currentIndex = 1;
				char readChar = m_document[currentIndex];
				while (readChar != '\r' && readChar != '\n' && readChar != '\t' && readChar != ' ' && readChar != ',' && readChar != '}' && readChar != ']')
				{
					++currentIndex;
					readChar = m_document[currentIndex];

				}

				m_token = m_document.Substr(0, currentIndex);
				m_document = m_document.Substr(currentIndex);
				
				readNextChar = false;
			}
			break;

			case '"':
			{
				uint32_t lastDoubleQuote = 0;
				uint32_t offset = 1;
				bool lastQuoteFound = false;
				while (!lastQuoteFound)
				{
					lastDoubleQuote = m_document.FindFirstOf('"', 1);
					char previousChar = m_document[lastDoubleQuote - 1];
					if (previousChar != '\\')
						break;

					offset = lastDoubleQuote + 1;
				}

				uint32_t endCharIndex = lastDoubleQuote + 1;
				m_token = m_document.Substr(0, endCharIndex);
				m_document = m_document.Substr(endCharIndex);
				readNextChar = false;
			}
			break;

			default:
			{
				// keyword like true, false, null
				int currentIndex = 1;
				char readChar = m_document[currentIndex];
				while (readChar != '\r' && readChar != '\n' && readChar != '\t' && readChar != ' ' && readChar != ',' && readChar != '}' && readChar != ']')
				{
					++currentIndex;
					readChar = m_document[currentIndex];
				}

				m_token = m_document.Substr(0, currentIndex);
				m_document = m_document.Substr(currentIndex);
				readNextChar = false;
			}
			break;
			}
		}

		return true;
	}

	void JsonDeserializer::ReadArray(JsonArray& output)
	{
		bool keepReading = true;
		while (keepReading)
		{
			ReadNextToken();
			char firstCharacter = m_token[0];

			if (firstCharacter == '{')
			{
				JsonObject* pNewObject = new JsonObject();
				output.AddElement(pNewObject);
				ReadObject(*pNewObject);
			}
			else if (firstCharacter == '-' || std::isdigit(firstCharacter))
			{
				double value = 0;
				std::from_chars(m_token.GetData(), m_token.GetData() + m_token.GetSize(), value);
				output.AddElement(value);
			}
			else if (firstCharacter == '[')
			{
				JsonArray* pArray = new JsonArray();
				output.AddElement(pArray);
				ReadArray(*pArray);
			}
			else if (firstCharacter == '"')
			{
				String str = m_token.Substr(1, m_token.GetSize() - 2);
				std::string value(str.GetData(), str.GetSize());
				output.AddElement(value);
			}
			else if (m_token == "true")
			{
				output.AddElement(true);
			}
			else if (m_token == "false")
			{
				output.AddElement(false);
			}

			ReadNextToken();
			firstCharacter = m_token[0];
			if (firstCharacter == ']')
				break;
		}
	}

	void JsonDeserializer::ReadObject(JsonObject& output)
	{
		//this is an object so read now the members
		bool keepReading = true;
		while (keepReading)
		{
			keepReading = ReadNextToken();
			String str = m_token.Substr(1, m_token.GetSize() - 2);
			std::string memberName(str.GetData(), str.GetSize());

			if (!keepReading)
				assert(false);

			keepReading = ReadNextToken(); // this is the colon separating the name and the value
			if (!keepReading)
				assert(false);

			keepReading = ReadNextToken();

			if (m_token[0] == '{')
			{
				JsonObject* pObject = output.AddObject(memberName);
				ReadObject(*pObject);
			}
			else if (m_token[0] == '-' || std::isdigit(m_token[0]))
			{
				double value = 0;
				std::from_chars(m_token.GetData(), m_token.GetData() + m_token.GetSize(), value);
				output.AddMember(memberName, value);
			}
			else if (m_token[0] == '[')
			{
				JsonArray* pArray = output.AddArray(memberName);
				ReadArray(*pArray);
			}
			else if (m_token[0] == '"')
			{
				String str = m_token.Substr(1, m_token.GetSize() - 2);
				std::string value(str.GetData(), str.GetSize());
				std::string unescapedString = UnescapeString(value);
				output.AddMember(memberName, unescapedString);
			}
			else if (m_token == "true")
			{
				output.AddMember(memberName, true);
			}
			else if (m_token == "false")
			{
				output.AddMember(memberName, false);
			}
			else if (m_token == "null")
			{
				output.AddNullMember(memberName);
			}

			ReadNextToken();
			if (m_token[0] == ',')
				keepReading = true;
			else if (m_token[0] == '}')
				keepReading = false;
		}
	}
}