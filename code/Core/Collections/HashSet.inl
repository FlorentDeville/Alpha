/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "3rdparty/xxh64/xxh64.hpp"

namespace Core
{
	template<class T, int Size> HashSet<T, Size>::HashSet()
	{ }

	template<class T, int Size> HashSet<T, Size>::~HashSet()
	{ }

	template<class T, int Size> bool HashSet<T, Size>::Insert(const T& value)
	{
		uint64_t hash = xxh64::hash(reinterpret_cast<const char*>(&value), sizeof(T), Size);
		uint64_t index = hash % Size;
		for (const T& existingValue : m_storage[index])
		{
			if (existingValue == value)
				return true;
		}

		m_storage[index].PushBack(value);
		return false;
	}

	template<class T, int Size> bool HashSet<T, Size>::Exists(const T& value) const
	{
		uint64_t hash = xxh64::hash(reinterpret_cast<const char*>(&value), sizeof(T), Size);
		uint64_t index = hash % Size;
		for (const T& existingValue : m_storage[index])
		{
			if (existingValue == value)
				return true;
		}

		return false;
	}
}
