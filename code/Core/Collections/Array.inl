/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

namespace Core
{
	template<typename T> Array<T>::Array()
		: BaseArray()
		, m_pStart(nullptr)
		, m_size(0)
		, m_reservedSize(0)
	{
	}

	template<typename T> Array<T>::Array(const Array& source)
	{
		*this = source;
	}

	template<typename T> Array<T>::Array(Array&& source)
	{
		*this = std::move(source);
	}

	template<typename T> Array<T>::~Array()
	{
		Clear();
	}

	template<typename T> void Array<T>::PushBack(const T& item)
	{
		if (m_reservedSize <= m_size)
		{
			if (m_reservedSize == 0)
				m_reservedSize = 10;
			else
				m_reservedSize = m_reservedSize + (m_reservedSize / 2) + 1;

			T* pNewArray = new T[m_reservedSize];
			for (uint32_t ii = 0; ii < m_size; ++ii)
				pNewArray[ii] = std::move(m_pStart[ii]);

			delete[] m_pStart;
			m_pStart = pNewArray;
		}

		m_pStart[m_size] = item;
		++m_size;
	}

	template<typename T> void Array<T>::Insert(const T& item, uint32_t position)
	{
		uint32_t newSize = m_size + 1;
		if (m_reservedSize < newSize)
			Reserve((m_reservedSize * 2) + 1);

		//there is a possible optimization here : Reserve will copy/move all the elements from the old array to the new array
		//and then below I copy/move again to make space at position. I could do it all in a single copy/move.
		for (uint32_t ii = m_size; ii > position; --ii)
			m_pStart[ii] = std::move(m_pStart[ii - 1]);

		m_pStart[position] = item;
		m_size++;
	}

	template<typename T> uint32_t Array<T>::GetSize() const
	{
		return m_size;
	}

	template<typename T> void* Array<T>::GetElement(uint32_t index)
	{
		assert(index < m_size);
		return &m_pStart[index];
	}

	template<typename T> const void* Array<T>::GetConstElement(uint32_t index) const
	{
		assert(index < m_size);
		return &m_pStart[index];
	}

	template<typename T> void Array<T>::RemoveElement(uint32_t index)
	{
		if (!IsValidIndex(index))
			return;

		Iterator it(m_pStart + index);
		Erase(it);
	}

	template<typename T> T* Array<T>::GetData()
	{
		return m_pStart;
	}

	template<typename T> const T* Array<T>::GetData() const
	{
		return m_pStart;
	}

	template<typename T> T& Array<T>::operator[](uint32_t index)
	{
		assert(index < m_size);
		return m_pStart[index];
	}

	template<typename T> const T& Array<T>::operator[](uint32_t index) const
	{
		assert(index < m_size);
		return m_pStart[index];
	}

	template<typename T> void Array<T>::operator=(const Array& source)
	{
		Clear();

		Resize(source.GetSize());

		for (uint32_t ii = 0; ii < m_size; ++ii)
			m_pStart[ii] = source[ii];
	}

	template<typename T> void Array<T>::operator=(Array&& source)
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
				m_pStart[ii] = value;
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
		for (uint32_t ii = 0; ii < m_size; ++ii)
			pNewArray[ii] = std::move(m_pStart[ii]);

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
	{
	}

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
