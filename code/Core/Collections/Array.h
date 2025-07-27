/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/BaseArray.h"

#include <assert.h>
#include <cstring>
#include <iterator>
#include <utility>

namespace Core
{
	template<typename T> class Array : public BaseArray
	{
	public:

		class Iterator
		{
		public:
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::forward_iterator_tag;

			Iterator(pointer pPtr);

			reference operator*() const;
			pointer operator->() const;

			Iterator& operator++();
			Iterator& operator++(int);

			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;

		private:
			pointer m_pPtr;
		};

		Array()
			: BaseArray()
			, m_pStart(nullptr)
			, m_size(0)
			, m_reservedSize(0)
		{}

		Array(const Array& source)
		{
			*this = source;
		}

		Array(Array&& source)
		{
			*this = std::move(source);
		}

		~Array()
		{
			Clear();
		}

		void PushBack(const T& item)
		{
			if (m_reservedSize <= m_size)
			{
				if (m_reservedSize == 0)
					m_reservedSize = 10;
				else
					m_reservedSize = m_reservedSize + (m_reservedSize / 2) + 1;
				
				T* pNewArray = new T[m_reservedSize];
				std::memcpy(pNewArray, m_pStart, sizeof(T) * m_size);

				delete[] m_pStart;
				m_pStart = pNewArray;
			}

			m_pStart[m_size] = item;
			++m_size;
		}

		uint32_t GetSize() const override
		{
			return m_size;
		}

		void* GetElement(uint32_t index) override
		{
			assert(index < m_size);
			return &m_pStart[index];
		}

		const void* GetConstElement(uint32_t index) const override
		{
			assert(index < m_size);
			return &m_pStart[index];
		}

		void RemoveElement(uint32_t index) override;

		T* GetData()
		{
			return m_pStart;
		}

		const T* GetData() const
		{
			return m_pStart;
		}

		T& operator[](uint32_t index)
		{
			assert(index < m_size);
			return m_pStart[index];
		}

		const T& operator[](uint32_t index) const
		{
			assert(index < m_size);
			return m_pStart[index];
		}

		void operator=(const Array& source)
		{
			Clear();

			Resize(source.GetSize());

			for (uint32_t ii = 0; ii < m_size; ++ii)
				m_pStart[ii] = source[ii];
		}

		void operator=(Array&& source)
		{
			//clean up existing memory
			for (uint32_t ii = 0; ii < m_size; ++ii)
				m_pStart[ii].~T();

			delete[] m_pStart;

			//now do the move
			m_pStart = source.m_pStart;
			m_size = source.m_size;
			m_reservedSize = source.m_reservedSize;

			source.m_pStart = nullptr;
			source.m_size = 0;
			source.m_reservedSize = 0;
		}

		T& Back();
		const T& Back() const;

		void Resize(uint32_t newSize) override;
		void Resize(uint32_t newSize, const T& value);

		void AddElement() override;

		void Reserve(uint32_t index, bool allowShrink = false);

		void Clear();

		//Erase the first element equal to value and return the number of element erased.
		template<typename U> uint32_t Erase(const U& value);

		//Erase the element at the position of the iterator.
		void Erase(Iterator& it);

		bool IsValidIndex(int32_t index) const;

		//function to work with stl algorithms
		Iterator begin();
		const Iterator begin() const;

		Iterator end();
		const Iterator end() const;

		const Iterator cbegin() const;
		const Iterator cend() const;

	private:
		uint32_t m_size; // number of element used
		uint32_t m_reservedSize; // number of element allocated
		T* m_pStart;

	};

	template<typename T> void Array<T>::RemoveElement(uint32_t index)
	{
		if (!IsValidIndex(index))
			return;

		Iterator it(m_pStart + index);
		Erase(it);
	}

	template<typename T> T& Array<T>::Back()
	{
		return m_pStart[m_size - 1];
	}

	template<typename T> const T& Array<T>::Back() const
	{
		return m_pStart[m_size - 1];
	}

	template<typename T> void Array<T>::Resize(uint32_t newSize)
	{
		if (newSize < m_size)
		{
			for (uint32_t ii = newSize; ii < m_size; ++ii)
				m_pStart[ii].~T();
		}

		if (m_reservedSize < newSize)
			Reserve(newSize);

		m_size = newSize;
	}

	template<typename T> void Array<T>::Resize(uint32_t newSize, const T& value)
	{
		uint32_t oldSize = m_size;
		Resize(newSize);

		if (m_size > oldSize)
		{
			for (uint32_t ii = oldSize; ii < m_size; ++ii)
			{
				// m_pStart is a T*, so I can just increase the pointer
				memcpy(m_pStart + ii, &value, sizeof(value));
			}
		}
	}

	template<typename T> void Array<T>::AddElement()
	{
		PushBack(T());
	}

	template<typename T> void Array<T>::Reserve(uint32_t newSize, bool allowShrink)
	{
		if (m_reservedSize > newSize && !allowShrink)
			return;

		if (m_size > newSize)
		{
			for (uint32_t ii = newSize; ii < m_size; ++ii)
				m_pStart[ii].~T();

			m_size = newSize;
		}

		T* pNewArray = new T[newSize];
		std::memcpy(pNewArray, m_pStart, sizeof(T) * m_size);

		delete[] m_pStart;
		m_pStart = pNewArray;

		m_reservedSize = newSize;
	}

	template<typename T> void Array<T>::Clear()
	{
		for (uint32_t ii = 0; ii < m_reservedSize; ++ii)
			m_pStart[ii].~T();

		delete[] m_pStart;

		m_reservedSize = 0;
		m_size = 0;
		m_pStart = nullptr;
	}

	template<typename T> template<typename U> uint32_t Array<T>::Erase(const U& value)
	{
		Iterator it = std::find(cbegin(), cend(), value);
		if (it == cend())
			return 0;
		
		int oldSize = GetSize();
		Erase(it);
		return oldSize - GetSize();
	}

	template<typename T> void Array<T>::Erase(Iterator& it)
	{
		//first delete the element
		T& pValue = *it;
		pValue.~T();

		//move the elements
		Iterator previousElement = it;
		Iterator ii = it;
		++ii;
		for (; ii != end(); ++ii, ++previousElement)
		{
			T& src = *ii;
			T& dst = *previousElement;

			dst = std::move(src);
		}

		--m_size;
	}

	template<typename T> bool Array<T>::IsValidIndex(int32_t index) const
	{
		if (index < 0 || index >= static_cast<int32_t>(m_size))
			return false;

		return true;
	}

	template<typename T> Array<T>::Iterator::Iterator(pointer pPtr)
		: m_pPtr(pPtr)
	{ }

	template<typename T> typename Array<T>::Iterator Array<T>::begin()
	{
		return Iterator(m_pStart);
	}

	template<typename T> typename const Array<T>::Iterator Array<T>::begin() const
	{
		return Iterator(m_pStart);
	}

	template<typename T> typename Array<T>::Iterator Array<T>::end()
	{
		return Iterator(m_pStart + m_size);
	}

	template<typename T> typename const Array<T>::Iterator Array<T>::end() const
	{
		return Iterator(m_pStart + m_size);
	}

	template<typename T> const typename Array<T>::Iterator Array<T>::cbegin() const
	{
		return Iterator(m_pStart);
	}

	template<typename T> const typename Array<T>::Iterator Array<T>::cend() const
	{
		return Iterator(m_pStart + m_size);
	}

	template<typename T> typename Array<T>::Iterator::reference Array<T>::Iterator::operator*() const
	{
		return *m_pPtr;
	}

	template<typename T> typename Array<T>::Iterator::pointer Array<T>::Iterator::operator->() const
	{
		return m_pPtr;
	}

	template<typename T> typename Array<T>::Iterator& Array<T>::Iterator::operator++()
	{
		++m_pPtr;
		return *this;
	}

	template<typename T> typename Array<T>::Iterator& Array<T>::Iterator::operator++(int)
	{
		Iterator temp = *this;
		++m_pPtr;
		return temp;
	}

	template<typename T> bool Array<T>::Iterator::operator==(const Iterator& other) const
	{
		return m_pPtr == other.m_pPtr;
	}

	template<typename T> bool Array<T>::Iterator::operator!=(const Iterator& other) const
	{
		return !(*this == other);
	}
}
