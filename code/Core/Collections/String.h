/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

namespace Core
{
	class StringView;

	class String : public Array<char>
	{
	public:
		String();
		String(char str);
		String(const char* str);
		String(const char* str, uint32_t length);
		String(const String& str);
		String(const StringView& view);
		String(String&& str);

		~String();

		String& operator=(const String& other);

		bool operator==(const String& other) const;
		bool operator==(const char* other) const;

		String Substr(uint32_t start, uint32_t length) const;

	private:
		static void Swap(String& src, String& dst);
	};
}
