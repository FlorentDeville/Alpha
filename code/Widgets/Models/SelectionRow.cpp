/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/SelectionRow.h"

#include <cassert>

namespace Widgets
{
	SelectionRow::SelectionRow(const ModelIndex& parent, int row)
		: m_parentIndex(parent)
		, m_row(row)
	{}
	
	SelectionRow::~SelectionRow()
	{}

	ModelIndex SelectionRow::GetParent() const
	{
		return m_parentIndex;
	}

	int SelectionRow::GetRow() const
	{
		return m_row;
	}

	bool SelectionRow::operator==(const SelectionRow& other) const
	{
		if (m_parentIndex == other.m_parentIndex && m_row == other.m_row)
			return true;

		return false;
	}

	bool SelectionRow::operator!=(const SelectionRow& other) const
	{
		return !(*this == other);
	}
}
