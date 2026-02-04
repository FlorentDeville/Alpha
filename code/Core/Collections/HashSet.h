/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

namespace Core
{
	// Unordered set. Cannot be serialized.
	template<class T, int Size = 1000> class HashSet
	{
	public:
		HashSet();
		~HashSet();

		bool Insert(const T& value);
		bool Exists(const T& value) const;

	private:
		Array<T> m_storage[Size];
	};
}

#include "Core/Collections/HashSet.inl"
