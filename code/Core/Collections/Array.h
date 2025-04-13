/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/BaseArray.h"

#include <assert.h>
#include <cstring>

namespace Core
{
	template<typename T> class Array : public BaseArray
	{
	public:
		Array()
			: BaseArray()
			, m_pStart(nullptr)
			, m_size(0)
			, m_reservedSize(0)
		{}

		Array(Array&& source)
		{
			*this = source;
		}

		~Array()
		{
			for (int ii = 0; ii < m_size; ++ii)
				m_pStart[m_size].~T();

			delete[] m_pStart;
			m_pStart = nullptr;

			m_size = 0;
			m_reservedSize = 0;
		}

		void PushBack(const T& item)
		{
			if (m_reservedSize <= m_size)
			{
				if (m_reservedSize == 0)
					m_reservedSize = 10;
				else
					m_reservedSize = m_reservedSize + (m_reservedSize / 2);
				
				T* pNewArray = new T[m_reservedSize];
				std::memcpy(pNewArray, m_pStart, sizeof(T) * m_size);

				delete[] m_pStart;
				m_pStart = pNewArray;
			}

			m_pStart[m_size] = item;
			++m_size;
		}

		int32_t GetSize() const override
		{
			return m_size;
		}

		void* GetElement(int32_t index) override
		{
			assert(index >= 0 && index < m_size);
			return &m_pStart[index];
		}

		const void* GetConstElement(int32_t index) const override
		{
			assert(index >= 0 && index < m_size);
			return &m_pStart[index];
		}

		T* GetData()
		{
			return m_pStart;
		}

		const T* GetData() const
		{
			return m_pStart;
		}

		T& operator[](int32_t index)
		{
			assert(index >= 0 && index < m_size);
			return m_pStart[index];
		}

		void operator=(Array&& source)
		{
			m_pStart = source.m_pStart;
			m_size = source.m_size;
			m_reservedSize = source.m_reservedSize;

			source.m_pStart = nullptr;
			source.m_size = 0;
			source.m_reservedSize = 0;
		}

		void Resize(int32_t newSize) override;

		void Reserve(int32_t index, bool allowShrink = false);

	private:
		int32_t m_size; // number of element used
		int32_t m_reservedSize; // number of element allocated
		T* m_pStart;

	};

	template<typename T> void Array<T>::Resize(int32_t newSize)
	{
		if (newSize < m_size)
		{
			for (int ii = newSize; ii < m_size; ++ii)
				m_pStart[ii].~T();
		}

		if (m_reservedSize < newSize)
			Reserve(newSize);

		m_size = newSize;
	}

	template<typename T> void Array<T>::Reserve(int32_t newSize, bool allowShrink)
	{
		if (m_reservedSize > newSize && !allowShrink)
			return;

		if (m_size > newSize)
		{
			for (int ii = newSize; ii < m_size; ++ii)
				m_pStart[ii].~T();

			m_size = newSize;
		}

		T* pNewArray = new T[newSize];
		std::memcpy(pNewArray, m_pStart, sizeof(T) * m_size);

		delete[] m_pStart;
		m_pStart = pNewArray;

		m_reservedSize = newSize;
	}
}
