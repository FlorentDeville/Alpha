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

	void SelectionModel::BeforeRemoveRows(int start, int count, const ModelIndex& parent)
	{
		std::list<SelectionRow>::const_iterator it = m_selectedRows.cbegin();
		while(it != m_selectedRows.cend())
		{
			const SelectionRow& selectedRow = *it;

			if (selectedRow.GetParent() != parent)
			{
				++it;
				continue;
			}

			ModelIndex startIndex = selectedRow.GetStartIndex();
			if (startIndex.GetRow() < start)
			{
				++it;
				continue;
			}

			//pretend a SelectionRow is only a single row for now
			if (startIndex.GetRow() >= start + count)
			{
				++it;
				continue;
			}

			m_deselectedRowsWhenDelete.push_back(selectedRow);
			it = m_selectedRows.erase(it);
		}

		std::vector<Widgets::SelectionRow> select;
		m_onSelectionChanged(select, m_deselectedRowsWhenDelete);
	}

	void SelectionModel::AfterRemoveRows(int start, int count, const ModelIndex& parent)
	{
		if (parent.IsValid())
			return;

		std::vector<Widgets::SelectionRow> select;

		//now the widgets are deleted so select the row after the row being deleted
		for (const SelectionRow& oldSelectedRow : m_deselectedRowsWhenDelete)
		{
			ModelIndex index = oldSelectedRow.GetStartIndex();
			const AbstractViewModel* pModel = index.GetConstModel();
			int rowCount = pModel->GetRowCount(parent);

			if (IsRowSelected(oldSelectedRow))
				continue;

			int oldRow = oldSelectedRow.GetStartIndex().GetRow();
			if (oldRow >= 0 && oldRow < rowCount)
			{
				m_selectedRows.push_back(oldSelectedRow);
				select.push_back(oldSelectedRow);
				continue;
			}

			if (oldRow >= rowCount && rowCount > 0)
			{
				ModelIndex index = pModel->GetIndex(rowCount - 1, 0, parent);
				m_selectedRows.push_back(SelectionRow(index, index));
				select.push_back(SelectionRow(index, index));
			}
		}

		std::vector<Widgets::SelectionRow> deselect;
		m_onSelectionChanged(select, deselect);

		m_deselectedRowsWhenDelete.clear();
	}
}
