/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "3rdparty/xxh64/xxh64.hpp"

#define MAKESID(LiteralString) [&]() { constexpr Core::Sid sid = Core::MakeSid(LiteralString); return sid; } ()
#define SID(LiteralString) Core::MakeSid(LiteralString)

namespace Core
{
	static constexpr uint32_t PRIME_SID = 2654435761U;

	// A Sid is a hashed string using xxh64. It is all constexpr but can be also used at runtime.
	// To create a compile time generated Sid, use the macro MAKESID.
	typedef uint64_t Sid;

	static constexpr Sid MakeSid()
	{
		return static_cast<Sid>(-1);
	}

	static constexpr Sid MakeSid(const std::string& str)
	{
		return static_cast<Sid>(xxh64::hash(str.c_str(), str.size(), PRIME_SID));
	}

	template<size_t N> static constexpr Sid MakeSid(const char(&str)[N])
	{
		//For compatibility between std::string and string literal.
		//String literal includes the \0 character in their size but std::string doesn't.
		return static_cast<Sid>(xxh64::hash(str, N-1, PRIME_SID));
	}

	std::string ToString(Sid sid);

	static constexpr Sid INVALID_SID = MakeSid();
}
