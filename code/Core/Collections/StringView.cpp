/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Collections/StringView.h"

namespace Core
{
	StringView::StringView()
		: m_pPtr(nullptr)
		, m_size(0)
	{ }

	StringView::StringView(const char* pPtr, uint32_t size)
		: m_pPtr(pPtr)
		, m_size(size)
	{ }

	StringView StringView::Substr(uint32_t start) const
	{
		return StringView(m_pPtr + start, m_size - start);
	}

	StringView StringView::Substr(uint32_t start, uint32_t length) const
	{
		return StringView(m_pPtr + start, length);
	}

	bool StringView::Empty() const
	{
		return m_size == 0;
	}

	char StringView::Front() const
	{
		return m_pPtr[0];
	}

	const char* StringView::GetData() const
	{
		return m_pPtr;
	}

	uint32_t StringView::GetSize() const
	{
		return m_size;
	}

	char StringView::operator[](uint32_t index) const
	{
		return m_pPtr[index];
	}

	uint32_t StringView::FindFirstOf(char c, uint32_t pos) const
	{
		for (uint32_t ii = pos; ii < m_size; ++ii)
		{
			if (m_pPtr[ii] == c)
				return ii;
		}

		return -1;
	}
}
