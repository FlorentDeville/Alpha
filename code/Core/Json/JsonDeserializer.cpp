/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonDeserializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"

#include <assert.h>

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

	void StripLeadingAndTrailingWhitespace(std::string& str)
	{
		size_t firstNonWhitespace = str.find_first_not_of(" \r\n\t");
		size_t lastNonWhitespace = str.find_last_not_of(" \r\n\t");

		str = str.substr(firstNonWhitespace, lastNonWhitespace - firstNonWhitespace + 1);
	}

	JsonDeserializer::JsonDeserializer()
		: m_stream()
		, m_line()
		, m_token()
	{}

	JsonDeserializer::~JsonDeserializer()
	{}

	bool JsonDeserializer::Deserialize(const std::string& input, JsonObject& output)
	{
		m_stream.str(input);

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
		if (m_line.eof() || m_line.rdbuf()->in_avail() == 0)
		{
			if (m_stream.eof() || m_stream.rdbuf()->in_avail() == 0)
			{
				m_token.clear();
				return false;
			}

			std::string line;
			std::getline(m_stream, line);
			StripLeadingAndTrailingWhitespace(line);

			if (line.empty())
				return ReadNextToken();

			m_line.clear();
			m_line.str(line);
		}

		char nextChar = m_line.peek();
		if (nextChar == '{')
		{
			std::getline(m_line, m_token, '{');
			m_token = "{";
		}
		else if (nextChar == '}')
		{
			std::getline(m_line, m_token, '}');
			m_token = "}";
		}
		else if (nextChar == '"')
		{
			//read the string
			std::getline(m_line, m_token, '"'); //start quote

			bool lastQuoteFound = false;
			while (!lastQuoteFound)
			{
				std::string tempToken;
				std::getline(m_line, tempToken, '"');

				m_token += tempToken;

				if (tempToken.size() > 0)
				{
					char escapeCharacter = tempToken[tempToken.size() - 1];
					if (escapeCharacter != '\\')
						lastQuoteFound = true;
					else
						m_token += "\"";
				}
			}
			
			m_token = "\"" + m_token + "\"";
		}
		else if (nextChar == ':')
		{
			std::getline(m_line, m_token, ':');
			m_token = ":";
		}
		else if (nextChar == '[')
		{
			std::getline(m_line, m_token, '[');
			m_token = "[";
		}
		else if (nextChar == ']')
		{
			std::getline(m_line, m_token, ']');
			m_token = "]";
		}
		else if (nextChar == ' ')
		{
			std::getline(m_line, m_token, ' ');
			return ReadNextToken();
		}
		else if (nextChar == '-' || std::isdigit(nextChar))
		{
			const int BUFFER_SIZE = 32;
			char buffer[BUFFER_SIZE] = { '\0' };

			buffer[0] = m_line.get();
			
			int currentIndex = 1;
			char readChar = m_line.peek();
			while (readChar != '\r' && readChar != '\n' && readChar != '\t' && readChar != ' ' && readChar != ',' && readChar != '}' && readChar != ']' && !m_line.eof())
			{
				assert(currentIndex < BUFFER_SIZE - 1);
				buffer[currentIndex] = m_line.get();
				readChar = m_line.peek();
				++currentIndex;

			}
			m_token = buffer;
		}
		else if (nextChar == ',')
		{
			m_token = m_line.get();
		}
		else
		{
			// keyword like true, false, null
			m_token = "";
			m_token += m_line.get();

			char readChar = m_line.peek();
			while (readChar != '\r' && readChar != '\n' && readChar != '\t' && readChar != ' ' && readChar != ',' && readChar != '}' && readChar != ']' && !m_line.eof())
			{
				m_token += m_line.get();
				readChar = m_line.peek();
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

			if (m_token[0] == '{')
			{
				JsonObject* pNewObject = new JsonObject();
				output.AddElement(pNewObject);
				ReadObject(*pNewObject);
			}
			else if (m_token[0] == '-' || std::isdigit(m_token[0]))
			{
				double value = std::stod(m_token);
				output.AddElement(value);
			}
			else if (m_token[0] == '[')
			{
				JsonArray* pArray = new JsonArray();
				output.AddElement(pArray);
				ReadArray(*pArray);
			}
			else if (m_token[0] == '"')
			{
				std::string value = m_token.substr(1, m_token.size() - 2);
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

			if (m_token[0] == ',')
			{
				keepReading = true;
			}
			else if (m_token[0] == ']')
			{
				keepReading = false;
			}
			else
			{
				assert(false);
			}
		}
	}

	void JsonDeserializer::ReadObject(JsonObject& output)
	{
		//this is an object so read now the members
		bool keepReading = true;
		while (keepReading)
		{
			keepReading = ReadNextToken();
			std::string memberName = m_token.substr(1, m_token.size() - 2);

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
				double value = std::stod(m_token);
				output.AddMember(memberName, value);
			}
			else if (m_token[0] == '[')
			{
				JsonArray* pArray = output.AddArray(memberName);
				ReadArray(*pArray);
			}
			else if (m_token[0] == '"')
			{
				std::string value = m_token.substr(1, m_token.size() - 2);
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