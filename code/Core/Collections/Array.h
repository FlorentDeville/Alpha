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

		Array();

		Array(const Array& source);

		Array(Array&& source);

		~Array();

		void PushBack(const T& item);

		void Insert(const T& item, uint32_t position);

		uint32_t GetSize() const override;

		void* GetElement(uint32_t index) override;

		const void* GetConstElement(uint32_t index) const override;

		void RemoveElement(uint32_t index) override;

		T* GetData();

		const T* GetData() const;

		T& operator[](uint32_t index);

		const T& operator[](uint32_t index) const;

		void operator=(const Array& source);

		void operator=(Array&& source);

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
}

#include "Core/Collections/Array.inl"
