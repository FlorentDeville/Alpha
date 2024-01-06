/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/SelectionRow.h"

#include <cassert>

namespace Widgets
{
	SelectionRow::SelectionRow(const ModelIndex& start, const ModelIndex& end)
		: m_startIndex(start)
		, m_endIndex(end)
	{}
	
	SelectionRow::~SelectionRow()
	{}

	ModelIndex SelectionRow::GetStartIndex() const
	{
		return m_startIndex;
	}

	ModelIndex SelectionRow::GetEndIndex() const
	{
		return m_endIndex;
	}

	ModelIndex SelectionRow::GetParent() const
	{
		return m_startIndex.GetParent();
	}

	bool SelectionRow::operator==(const SelectionRow& other) const
	{
		if (m_startIndex == other.m_startIndex && m_endIndex == other.m_endIndex)
			return true;

		return false;
	}

	bool SelectionRow::operator!=(const SelectionRow& other) const
	{
		return !(*this == other);
	}
}
