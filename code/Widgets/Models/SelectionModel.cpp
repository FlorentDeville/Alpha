/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/SelectionModel.h"

#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/SelectionRow.h"

namespace Widgets
{
	SelectionModel::SelectionModel()
		: m_selectedRows()
	{}

	SelectionModel::~SelectionModel()
	{}

	bool SelectionModel::IsRowSelected(const SelectionRow& row) const
	{
		for (const SelectionRow& selectedRow : m_selectedRows)
		{
			if (selectedRow == row)
				return true;
		}

		return false;
	}

	const std::list<SelectionRow>& SelectionModel::GetSelectedRows() const
	{
		return m_selectedRows;
	}

	void SelectionModel::SetSelectionRow(const SelectionRow& row)
	{
		std::vector<SelectionRow> deselect;
		deselect.reserve(m_selectedRows.size());
		for (SelectionRow& oldSelectedRow : m_selectedRows)
			deselect.push_back(oldSelectedRow);

		m_selectedRows.clear();
		m_selectedRows.push_back(row);

		std::vector<SelectionRow> select;
		select.push_back(row);

		m_onSelectionChanged(select, deselect);
	}

	void SelectionModel::SelectRow(const SelectionRow& row)
	{
		if (IsRowSelected(row))
			return;

		m_selectedRows.push_back(row);

		std::vector<SelectionRow> select;
		select.push_back(row);
		std::vector<SelectionRow> deselect;
		m_onSelectionChanged(select, deselect);
	}

	void SelectionModel::DeselectRow(const SelectionRow& row)
	{
		std::vector<SelectionRow> select;
		std::vector<SelectionRow> deselect;

		bool found = false;
		for(std::list<SelectionRow>::const_iterator it = m_selectedRows.cbegin(); it != m_selectedRows.cend(); ++it)
		{
			const SelectionRow& selectedRow = *it;

			if (selectedRow != row)
				continue;

			deselect.push_back(selectedRow);
			m_selectedRows.erase(it);
			found = true;
			break;
		}

		if (found)
			m_onSelectionChanged(select, deselect);
	}

	void SelectionModel::CommitInsertRows(int start, int count, const ModelIndex& parent)
	{
		for (std::list<SelectionRow>::iterator it = m_selectedRows.begin(); it != m_selectedRows.end(); ++it)
		{
			SelectionRow& selectedRow = *it;

			if (selectedRow.GetParent() != parent)
				continue;

			ModelIndex startIndex = selectedRow.GetStartIndex();
			if (startIndex.GetRow() < start)
				continue;

			ModelIndex endIndex = selectedRow.GetEndIndex();

			int newRow = startIndex.GetRow() + count;
			ModelIndex newStart = startIndex.GetIndex(newRow, 0);
			ModelIndex newEnd = startIndex.GetIndex(newRow, endIndex.GetColumn());
			
			selectedRow = SelectionRow(newStart, newEnd);
		}
	}
}
