/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Collections/String.h"

#include "Core/Collections/StringView.h"

#include <utility>

namespace Core
{
	String::String()
		: Array<char>()
	{ }

	String::String(char str)
		: String(&str, 1)
	{ }

	String::String(const char* str, uint32_t length)
	{
		Reserve(length + 1, true);
		memcpy(m_pStart, str, length);
		m_pStart[length] = '\0';
		m_size = length;
	}

	String::String(const char* str)
		: String(str, static_cast<uint32_t>(strlen(str)))
	{ }

	String::String(const String& str)
		: String(str.GetData(), str.GetSize())
	{ }

	String::String(const StringView& view)
		: String(view.GetData(), view.GetSize())
	{ }

	String::String(String&& str)
	{
		Swap(*this, str);
	}

	String::~String()
	{ }

	String& String::operator=(const String& other)
	{
		String copy(other);
		Swap(*this, copy);
		return *this;
	}

	bool String::operator==(const String& other) const
	{
		if (other.GetSize() != m_size)
			return false;

		int res = memcmp(other.GetData(), m_pStart, m_size);
		return res == 0;
	}

	bool String::operator==(const char* other) const
	{
		size_t length = strlen(other);
		if (length != m_size)
			return false;

		int res = memcmp(m_pStart, other, length);
		return res == 0;
	}

	String String::Substr(uint32_t start, uint32_t length) const
	{
		return String(m_pStart + start, length);
	}

	void String::Swap(String& src, String& dst)
	{
		std::swap(src.m_size, dst.m_size);
		std::swap(src.m_reservedSize, dst.m_reservedSize);
		std::swap(src.m_pStart, dst.m_pStart);
	}
}
